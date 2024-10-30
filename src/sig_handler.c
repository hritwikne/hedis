#include <stdlib.h>

#include "../include/context.h"
#include "../include/sig_handler.h"

static Context *ctx;

void set_context(Context *context) {
    ctx = context;
}

void handle_sigint(int sig) {
    close(ctx->epoll_fd);

    for (int i=0; i < MAX_CLIENTS; i++) {
        if (ctx->client_sockets[i] != -1) {
            close(ctx->client_sockets[i]);
            ctx->client_sockets[i] = -1;
        }
    }

    close(ctx->server_fd);
    free(ctx);
    exit(EXIT_SUCCESS);
}

void handle_sigsegv(int sig) {

}
