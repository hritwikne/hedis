#include "../include/context.h"
#include "../include/mem_utils.h"
#include "../include/sig_handler.h"

void handle_sigint(int sig) {
    Context *ctx = get_context();
    ctx->terminate_sig = 1;

    close(ctx->epoll_fd);
    close(ctx->server_fd);
    
    pthread_join(ctx->event_loop_thread, NULL);
    pthread_cancel(ctx->compaction_thread);
    pthread_join(ctx->compaction_thread, NULL);

    pthread_cancel(ctx->expiry_monitor_thread);
    pthread_join(ctx->expiry_monitor_thread, NULL);

    destroy_ht(ctx->table);
    destroy_pq(ctx->ttl_pq);
    destroy_pq(ctx->freq_pq);
    
    cleanup_allocator();
    exit(EXIT_SUCCESS);
}

void handle_sigsegv(int sig) {
    Context *ctx = get_context();
    ctx->terminate_sig = 1;

    close(ctx->epoll_fd);
    close(ctx->server_fd);

    pthread_cancel(ctx->event_loop_thread); 
    pthread_join(ctx->event_loop_thread, NULL);

    pthread_cancel(ctx->compaction_thread);
    pthread_join(ctx->compaction_thread, NULL);

    pthread_cancel(ctx->expiry_monitor_thread);
    pthread_join(ctx->expiry_monitor_thread, NULL);

    destroy_ht(ctx->table);
    destroy_pq(ctx->ttl_pq);
    destroy_pq(ctx->freq_pq);
    
    cleanup_allocator();
    exit(EXIT_SUCCESS);
}
