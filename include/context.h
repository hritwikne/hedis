#ifndef CONTEXT_H
#define CONTEXT_H

#define MAX_EVENTS 10

#include <pthread.h>
#include <sys/epoll.h>
#include <netinet/in.h>

typedef struct {
    int epoll_fd;
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    int addrlen;
    int server_fd;
    struct sockaddr_in address;

    pthread_t event_loop_thread;
    int terminate_event_loop;
} Context;

#endif
