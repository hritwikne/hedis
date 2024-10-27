#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#include "../include/thread_pool.h"

// worker function that runs in each thread
void* worker(void* arg) {
    thread_pool_t* pool = (thread_pool_t*) arg;

    while (true) {
        pthread_mutex_lock(&pool->mutex);

        // wait for a task to be available or the pool to be stopped
        while (pool->task_count == 0 && !pool->stop) {
            pthread_cond_wait(&pool->cond, &pool->mutex); // unlocks mutex
        }

        // if pool is being stopped, exit the loop
        if (pool->stop) {
            pthread_mutex_unlock(&pool->mutex); break;
        }

        // get the next task from the queue and decrease task count
        task_t task = pool->tasks[--pool->task_count];
        pthread_mutex_unlock(&pool->mutex);

        // execute the task's function with its argument
        task.function(task.args);
    }

    return NULL;
}

void thread_pool_init(thread_pool_t* pool) {
    pool->task_count = 0;
    pool->stop = false;
    
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    // create & start threads
    for (int i=0; i < NUM_THREADS; i++) {
        pthread_create(&pool->threads[i], NULL, worker, pool);
    }
}

void thread_pool_add_task(thread_pool_t* pool, void (*function)(task_args_t*), task_args_t* args) {
    pthread_mutex_lock(&pool->mutex);

    // checking if there's space for more tasks, and add it to task queue
    if (pool->task_count < MAX_TASKS) {
        pool->tasks[pool->task_count++] = (task_t){function, args};
        pthread_cond_signal(&pool->cond); // signal a waiting worker thread to wake up
    }

    pthread_mutex_unlock(&pool->mutex);
}

void thread_pool_destroy(thread_pool_t* pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->stop = true;
    pthread_cond_broadcast(&pool->cond); // wake up all worker threads
    pthread_mutex_unlock(&pool->mutex);

    // wait for all worker threads to finish
    for (int i=0; i < NUM_THREADS; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // clean up mutex and condition variable
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);
}