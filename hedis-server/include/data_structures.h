#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "global_includes.h"

typedef struct Node {
    char *key;
    char *value;
    struct Node *next; // for chaining
    
    time_t ttl;
    
    int freq;
    int has_ttl;
} Node;

typedef struct {
    Node **heap;
    size_t size; // no. of elements in heap
    size_t capacity;
    pthread_mutex_t mutex;
} Priority_Queue;

typedef struct {
    pthread_mutex_t mutex;

    Priority_Queue *ttl_pq;
    Priority_Queue *freq_pq;
    
    Node **buckets;
    size_t size;
    size_t count;
} Hash_Table;

#endif