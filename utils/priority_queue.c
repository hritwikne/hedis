#include "../include/priority_queue.h"

Priority_Queue* create_pq(size_t initial_capacity) {
    Priority_Queue *pq = allocate(sizeof(Priority_Queue), (void**)&pq);
    
    pq->size = 0;
    pq->capacity = initial_capacity;
    pq->heap = allocate(initial_capacity * sizeof(Priority_Queue *), (void**)&pq->heap);
    pthread_mutex_init(&pq->mutex, NULL);

    return pq;
}

void destroy_pq(Priority_Queue *pq) {
    if (pq == NULL) return;
    
    pthread_mutex_destroy(&pq->mutex);
    deallocate(pq->heap);
    deallocate(pq);
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
        swap_nodes(pq->heap[index], pq->heap[smallest]);
        heapify(pq, smallest);
    }
}

void push_pq(Priority_Queue *pq, Node *node) {
    if (pq == NULL) return;
    lock(pq->mutex);

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
            swap_nodes(pq->heap[current], pq->heap[parent]);
            current = parent;
        } 
        else break;
    }

    unlock(pq->mutex);
}

Node* pop_pq(Priority_Queue *pq) {
    if (pq == NULL || pq->size == 0) return NULL;
    lock(pq->mutex);

    Node *root = pq->heap[0];
    pq->heap[0] = pq->heap[pq->size-1];
    pq->size--;

    heapify(pq, 0);
    unlock(pq->mutex);

    return root;
}

Node* peek_pq(Priority_Queue *pq) {
    if (pq == NULL) return NULL;

    lock(pq->mutex);
    Node *root = (pq->size > 0) ? pq->heap[0] : NULL;
    unlock(pq->mutex);

    return root;
}

int node_exists_pq(Priority_Queue *pq, Node *node) {
    if (pq == NULL || node == NULL) return -1;
    lock(pq->mutex);

    for (size_t i=0; i < pq->size; i++) {
        if (pq->heap[i] == node) {
            unlock(pq->mutex);
            return 1;
        }
    }

    unlock(pq->mutex);
    return -1;
} 

void delete_node_pq(Priority_Queue *pq, Node* node) {
    if (pq == NULL || node == NULL) return;
    lock(pq->mutex);

    for (size_t i=0; i < pq->size; i++) {
        if (pq->heap[i] == node) {
            pq->heap[i] = pq->heap[pq->size-1];
            pq->size--;
            heapify(pq, i);
            break;
        }
    }

    unlock(pq->mutex);
}