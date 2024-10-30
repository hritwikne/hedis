#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/context.h"
#include "../include/sig_handler.h"

static Context *ctx;

void set_context(Context *context) {
    ctx = context;
}

void handle_sigint(int sig) {
    ctx->terminate_event_loop = 1;

    close(ctx->epoll_fd);
    close(ctx->server_fd);

    pthread_join(ctx->event_loop_thread, NULL);
    
    free(ctx);
    exit(EXIT_SUCCESS);
}

void handle_sigsegv(int sig) {
    ctx->terminate_event_loop = 1;

    close(ctx->epoll_fd);
    close(ctx->server_fd);

    pthread_cancel(ctx->event_loop_thread);
    
    free(ctx);
    exit(EXIT_SUCCESS);
}
