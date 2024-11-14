#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "global_includes.h"

typedef struct {
    int server_fd;
    int epoll_fd;
    struct epoll_event *events;
    int max_events;
    struct sockaddr_in *address;
    int *addrlen;
    int *terminate_sig;
} EventLoopArgs;

void initialize_epoll_server(int server_fd, int *epoll_fd, struct epoll_event *event);
void *run_event_loop(void *arg);

#endif