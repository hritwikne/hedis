#include "../include/event_loop.h"
#include "../include/msg_handler.h"
#include "../include/conn_handler.h"

void init_epoll_server() {
    Context *ctx = get_context();
    int epoll_fd = epoll_create1(0);

    if (epoll_fd < 0) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    ctx->epoll_fd = epoll_fd;
    ctx->event.events = EPOLLIN; // for incoming events (like read)
    ctx->event.data.fd = ctx->server_fd;

    int epoll_ctl_res = epoll_ctl(
        epoll_fd,
        EPOLL_CTL_ADD,
        ctx->server_fd,
        &ctx->event
    );

    if (epoll_ctl_res < 0) {
        perror("Failed to add server fd to epoll instance.");
        exit(EXIT_FAILURE);
    }
}


void* run_event_loop() {
    printf("-Running event loop-\n");
    Context *ctx = get_context();

    while (1) {
        int num_events = epoll_wait(
            ctx->epoll_fd, 
            ctx->events, 
            MAX_EVENTS, 
            5000
        );

        if (num_events < 0) {
            perror("epoll_wait failed");
            exit(EXIT_FAILURE);
        } 
        else if (num_events == 0 && ctx->terminate_sig == 1) {
            break;
        }

        for (int i = 0; i < num_events; i++) {
            if (ctx->events[i].data.fd == ctx->server_fd) {
                // the event is on server_fd - new connection
                accept_connections(
                    ctx->server_fd,
                    ctx->epoll_fd,
                    &ctx->address,
                    (socklen_t *)&ctx->addrlen
                );
            }
            
            else {
                // the event is on client_fd - new message
                int client_fd = ctx->events[i].data.fd;
                accept_messages(client_fd);
            }
        }
    }
    
    printf("-Exiting event loop-\n");
    return NULL;
}