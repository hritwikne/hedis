#ifndef CONTEXT_H
#define CONTEXT_H

#define MAX_EVENTS 10

#include "global_includes.h"
#include "hash_table.h"
#include "priority_queue.h"

typedef struct {
    int epoll_fd;
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    int addrlen;
    int server_fd;
    struct sockaddr_in address;

    int terminate_sig;
    pthread_t event_loop_thread;
    pthread_t compaction_thread;
    pthread_t expiry_monitor_thread;

    Hash_Table *table;
    Priority_Queue *ttl_pq;
    Priority_Queue *freq_pq;
} Context;

#endif
