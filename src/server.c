#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "../include/socket.h"
#include "../include/server.h"
#include "../include/context.h"
#include "../include/constants.h"
#include "../include/mem_utils.h"
#include "../include/event_loop.h"
#include "../include/hash_table.h"
#include "../include/sig_handler.h"
#include "../include/priority_queue.h"

static Context *ctx = NULL;

void print_startup_info(int port) {
    printf("\nHEDIS v0.1\n");
    printf("Listening on port %d..\n", port);
    printf(
        "Press Ctrl+C once to exit.\n"
        "Note: the program may take up to 5 seconds for cleanup to shut down completely.\n\n"
    );
}

void create_event_loop_thread() {
    EventLoopArgs *args = malloc(sizeof(EventLoopArgs));
    
    if (args == NULL) {
        perror("Failed to allocate memory for EventLoopArgs");
        exit(EXIT_FAILURE);
    }

    args->server_fd = ctx->server_fd;
    args->epoll_fd = ctx->epoll_fd;
    args->events = ctx->events;
    args->max_events = MAX_EVENTS;
    args->address = &ctx->address;
    args->addrlen = &ctx->addrlen;
    args->terminate_sig = &ctx->terminate_sig;

    int thread_creation_res = pthread_create(
        &ctx->event_loop_thread, 
        NULL, 
        run_event_loop, 
        args
    );

    if (thread_creation_res < 0) {
        perror("Failed to create a thread for event loop");
        free(args);
        exit(EXIT_FAILURE);
    }
}

void create_compaction_thread() {
    int thread_creation_res = pthread_create(
        &ctx->compaction_thread, 
        NULL, 
        compact_memory, 
        NULL
    );

    if (thread_creation_res < 0) {
        perror("Failed to create a thread for compaction process");
        exit(EXIT_FAILURE);
    }
}

void context_init() {
    ctx = malloc(sizeof(Context));

    if (ctx == NULL) {
        perror("Failed to allocate memory for Context");
        exit(EXIT_FAILURE);
    }

    set_context(ctx); // in sig_handler file
}

void create_expiry_monitor_thread() {
    int thread_creation_res = pthread_create(
        &ctx->expiry_monitor_thread, 
        NULL, 
        expiry_monitor, 
        &ctx->table
    );

    if (thread_creation_res < 0) {
        perror("Failed to create a thread for Expiry Monitor");
        exit(EXIT_FAILURE);
    }
}

void start_server(int port) {
    // set up custom mem allocator
    init_allocator();
    create_compaction_thread();
    
    // set up app state variables
    context_init();
    ctx->terminate_sig = 0;
    ctx->server_fd = create_socket();
    ctx->addrlen = sizeof(ctx->address);

    // register signal handlers for crashes    
    signal(SIGINT, handle_sigint);
    signal(SIGSEGV, handle_sigsegv);

    // setup data structures and related thread
    ctx->table = create_table(HASH_TABLE_SIZE);
    create_expiry_monitor_thread();

    // bind and listen to a socket
    set_socket_options(ctx->server_fd);
    bind_socket(ctx->server_fd, &ctx->address, port);
    listen_for_connections(ctx->server_fd);
    print_startup_info(port);

    // event loop setup for client conns
    initialize_epoll_server(ctx->server_fd, &ctx->epoll_fd, &ctx->event);
    create_event_loop_thread();

    pthread_join(ctx->event_loop_thread, NULL);
    pthread_join(ctx->compaction_thread, NULL);
    pthread_join(ctx->expiry_monitor_thread, NULL);
}