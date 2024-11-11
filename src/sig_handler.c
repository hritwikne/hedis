#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/context.h"
#include "../include/mem_utils.h"
#include "../include/sig_handler.h"

static Context *ctx;

void set_context(Context *context) {
    ctx = context;
}

void handle_sigint(int sig) {
    ctx->terminate_sig = 1;

    close(ctx->epoll_fd);
    close(ctx->server_fd);

    pthread_join(ctx->event_loop_thread, NULL);
    pthread_cancel(ctx->compaction_thread);
    
    free(ctx);
    cleanup_allocator();
    exit(EXIT_SUCCESS);
}

void handle_sigsegv(int sig) {
    ctx->terminate_sig = 1;

    close(ctx->epoll_fd);
    close(ctx->server_fd);

    // todo: cancelling thread leads to memory leaks, handle this?
    // have to close client sockets and free (args) in event loop
    pthread_cancel(ctx->event_loop_thread); 
    pthread_cancel(ctx->compaction_thread);
    
    free(ctx);
    cleanup_allocator();
    exit(EXIT_SUCCESS);
}
