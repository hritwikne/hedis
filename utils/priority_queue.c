#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/hash_table.h"
#include "../include/priority_queue.h"

Priority_Queue* create_pq(size_t initial_capacity) {
    Priority_Queue *pq = malloc(sizeof(Priority_Queue));
    
    pq->size = 0;
    pq->capacity = initial_capacity;
    pq->heap = malloc(initial_capacity * sizeof(Priority_Queue *));
    pthread_mutex_init(&pq->mutex, NULL);

    return pq;
}

void destroy_pq(Priority_Queue *pq) {
    if (pq == NULL) return;
    
    pthread_mutex_destroy(&pq->mutex);
    free(pq->heap);
    free(pq);
}

void heapify(Priority_Queue *pq, size_t index) {
    if (pq == NULL) return;
    size_t smallest = index;
    size_t left = (2 * index + 1);
    size_t right = (2 * index + 2);

    if (left < pq->size && pq->heap[left]->ttl < pq->heap[smallest]->ttl) {
        smallest = left;
    }

    if (right < pq->size && pq->heap[right]->ttl < pq->heap[smallest]->ttl) {
        smallest = right;
    }

    if (smallest != index) {
        // swap them
        Node *temp = pq->heap[index];
        pq->heap[index] = pq->heap[smallest];
        pq->heap[smallest] = temp;

        heapify(pq, smallest);
    }
}

void push_pq(Priority_Queue *pq, Node *node) {
    if (pq == NULL) return;
    pthread_mutex_lock(&pq->mutex);

    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        pq->heap = realloc(pq->heap, pq->capacity * sizeof(Node *));
    }

    pq->heap[pq->size] = node;
    size_t current = pq->size;
    pq->size++;

    while (current > 0) {
        size_t parent = (current - 1) / 2;
        time_t new_ttl = pq->heap[current]->ttl;
        time_t parent_ttl = pq->heap[parent]->ttl;

        if (new_ttl < parent_ttl) {
            // swap with parent
            Node *temp = pq->heap[current];
            pq->heap[current] = pq->heap[parent];
            pq->heap[parent] = temp;

            current = parent;
        } 
        else break;
    }

    pthread_mutex_unlock(&pq->mutex);
}

Node* pop_pq(Priority_Queue *pq) {
    if (pq == NULL || pq->size == 0) return NULL;
    pthread_mutex_lock(&pq->mutex);

    Node *root = pq->heap[0];
    pq->heap[0] = pq->heap[pq->size-1];
    pq->size--;

    heapify(pq, 0);
    pthread_mutex_unlock(&pq->mutex);

    return root;
}

Node* peek_pq(Priority_Queue *pq) {
    if (pq == NULL) return NULL;

    pthread_mutex_lock(&pq->mutex);
    Node *root = (pq->size > 0) ? pq->heap[0] : NULL;
    pthread_mutex_unlock(&pq->mutex);

    return root;
}

void delete_node_pq(Priority_Queue *pq, Node* node) {
    if (pq == NULL || node == NULL) return;
    pthread_mutex_lock(&pq->mutex);

    for (size_t i=0; i < pq->size; i++) {
        if (pq->heap[i] == node) {
            pq->heap[i] = pq->heap[pq->size-1];
            pq->size--;
            heapify(pq, i);
            break;
        }
    }

    pthread_mutex_unlock(&pq->mutex);
}