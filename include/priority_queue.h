#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "hash_table.h"

typedef struct {
    Node **heap;
    size_t size; // no. of elements in heap
    size_t capacity;
} Priority_Queue;

Priority_Queue* create_pq(size_t initial_capacity);
void destroy_pq(Priority_Queue *pq);
void push(Priority_Queue *pq, Node *node);
Node* pop(Priority_Queue *pq);
Node* peek(Priority_Queue *pq);
void heapify(Priority_Queue *pq, size_t index);
void delete_node_from_pq(Priority_Queue *pq, Node* node);

#endif