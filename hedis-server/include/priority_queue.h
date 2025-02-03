#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "utility.h"
#include "mem_utils.h"
#include "global_includes.h"
#include "data_structures.h"

Node* pop_pq(Priority_Queue *pq);
Node* peek_pq(Priority_Queue *pq);
void destroy_pq(Priority_Queue *pq);
void push_pq(Priority_Queue *pq, Node *node);
void heapify(Priority_Queue *pq, size_t index);
int node_exists_pq(Priority_Queue *pq, Node *node);
Priority_Queue* create_pq(size_t initial_capacity);
void delete_node_pq(Priority_Queue *pq, Node* node);

#endif