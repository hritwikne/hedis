#include "../include/socket.h"
#include "../include/server.h"
#include "../include/context.h"
#include "../include/mem_utils.h"
#include "../include/event_loop.h"
#include "../include/hash_table.h"
#include "../include/sig_handler.h"
#include "../include/utility.h"


void print_startup_info(int port) {
    printf("\nHEDIS v0.1\n");
    printf("Listening on port %d..\n", port);
    printf(
        "Press Ctrl+C once to exit.\n"
        "Note: the program may take up to 5 seconds for cleanup to shut down completely.\n\n"
    );
}

void start_server(int port) {
    Context *ctx = get_context();
    ctx->terminate_sig = 0;
    ctx->server_fd = create_socket();
    ctx->addrlen = sizeof(ctx->address);

    set_socket_options(ctx->server_fd);
    bind_socket(ctx->server_fd, &ctx->address, port);
    listen_for_connections(ctx->server_fd);

    init_epoll_server();
    init_mem_allocator();

    ctx->table = create_table(HASH_TABLE_SIZE);
    
    spawn_thread(ctx->event_loop_thread, run_event_loop);
    spawn_thread(ctx->expiry_monitor_thread, expiry_monitor);
    spawn_thread(ctx->compaction_thread, compact_memory);

    // register signal handlers for interrupts/crashes    
    signal(SIGINT, handle_sigint);
    signal(SIGSEGV, handle_sigsegv);

    print_startup_info(port);

    pthread_join(ctx->event_loop_thread, NULL);
    pthread_join(ctx->compaction_thread, NULL);
    pthread_join(ctx->expiry_monitor_thread, NULL);
}