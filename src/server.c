#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "../include/socket.h"
#include "../include/server.h"
#include "../include/context.h"
#include "../include/event_loop.h"
#include "../include/sig_handler.h"

static Context *ctx = NULL;

void create_event_loop_thread() {
    // building some arguments to pass to eventloop function
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

    // thread creation
    int thread_creation_res = pthread_create(
        ctx->event_loop_thread, 
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

void start_server(int port) {
    ctx = malloc(sizeof(Context));

    if (ctx == NULL) {
        perror("Failed to allocate memory for Context");
        exit(EXIT_FAILURE);
    }

    set_context(ctx); // in sig_handler file
    signal(SIGINT, handle_sigint);
    signal(SIGSEGV, handle_sigsegv);

    ctx->server_fd = create_socket();
    ctx->addrlen = sizeof(ctx->address);

    set_socket_options(ctx->server_fd);
    bind_socket(ctx->server_fd, &ctx->address, port);
    listen_for_connections(ctx->server_fd);

    printf("\nHEDIS v0.1\n");
    printf("PORT %d: Listening..\n", port);
    printf("Press Ctrl+C to exit.\n");

    initialize_epoll_server(ctx->server_fd, &ctx->epoll_fd, &ctx->event);
    create_event_loop_thread();
}