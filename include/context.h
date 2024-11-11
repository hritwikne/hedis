#ifndef CONTEXT_H
#define CONTEXT_H

#define MAX_EVENTS 10
#define COMPACTION_INTERVAL_SECONDS 3600
#define MEM_POOL_SIZE_MB 1024 * 1024 // 1mb

#include <pthread.h>
#include <sys/epoll.h>
#include <netinet/in.h>

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
    pthread_t auto_deletion_thread;

    Hash_Table ht;
    Priority_Queue pq;
} Context;

#endif
