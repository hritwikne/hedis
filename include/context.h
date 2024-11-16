#ifndef CONTEXT_H
#define CONTEXT_H

#include "constants.h"
#include "global_includes.h"
#include "data_structures.h"

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

    size_t total_memory;
    size_t used_memory;
    size_t free_memory;
} Context;

Context* get_context();

#endif
