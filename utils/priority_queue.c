#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/priority_queue.h"

Priority_Queue* create_pq(size_t initial_capacity) {
    Priority_Queue *pq = malloc(sizeof(Priority_Queue));
    pq->heap = malloc(initial_capacity * sizeof(Priority_Queue *));
    pq->size = 0;
    pq->capacity = initial_capacity;
    return pq;
}

void destroy_pq(Priority_Queue *pq) {
    if (pq == NULL) return;
    free(pq->heap);
    free(pq);
}

void heapify(Priority_Queue *pq, size_t index) {
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

void push(Priority_Queue *pq, Node *node) {
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
}

Node* pop(Priority_Queue *pq) {
    if (pq->size == 0) return NULL;

    Node *root = pq->heap[0];
    pq->heap[0] = pq->heap[pq->size-1];
    pq->size--;

    heapify(pq, 0);
    return root;
}

Node* peek(Priority_Queue *pq) {
    if (pq->size == 0) return NULL;
    return pq->heap[0];
}

void delete_node_from_pq(Priority_Queue *pq, Node* node) {
    for (size_t i=0; i < pq->size; i++) {
        if (pq->heap[i] == node) {
            pq->heap[i] = pq->heap[pq->size-1];
            pq->size--;
            heapify(pq, i);
            break;
        }
    }
}