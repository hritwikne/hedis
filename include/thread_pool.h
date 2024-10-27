#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>

#include "stack.h"

#define MAX_TASKS 10
#define NUM_THREADS 4

// structure to represent the arguments passed to task function
typedef struct {
    int client_no;
    int* client_sockets;
    Stack* stack;
} task_args_t;

// structure to represent a task
typedef struct {
    void (*function)(task_args_t* arg);
    task_args_t* args;
} task_t;

// structure to represent the thread pool
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    task_t tasks[MAX_TASKS];
    int task_count;
    bool stop;
    pthread_t threads[NUM_THREADS];
} thread_pool_t;

// function prototypes
void thread_pool_init(thread_pool_t* pool);
void thread_pool_add_task(thread_pool_t* pool, void (*function)(task_args_t*), task_args_t* args);
void thread_pool_destroy(thread_pool_t* pool); // cleans up thread

#endif