#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "utility.h"
#include "global_includes.h"

struct Node; // forward declaration

typedef struct {
    Node **heap;
    size_t size; // no. of elements in heap
    size_t capacity;
    pthread_mutex_t mutex;
} Priority_Queue;

Node* pop_pq(Priority_Queue *pq);
Node* peek_pq(Priority_Queue *pq);
void destroy_pq(Priority_Queue *pq);
void push_pq(Priority_Queue *pq, Node *node);
void heapify(Priority_Queue *pq, size_t index);
int node_exists_pq(Priority_Queue *pq, Node *node);
Priority_Queue* create_pq(size_t initial_capacity);
void delete_node_pq(Priority_Queue *pq, Node* node);

#endif