#include "../include/utility.h"

// djb2 algorithm
unsigned int hash(const char *key, size_t size) {
    int c;
    unsigned long hash = 5381;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash + c); // in effect: (hash * 33) + c;
    }
    
    return hash % size;
}

void lock(pthread_mutex_t mutex) {
    pthread_mutex_lock(&mutex);
}

void unlock(pthread_mutex_t mutex) {
    pthread_mutex_unlock(&mutex);
}

void swap_nodes(Node *node1, Node *node2) {
    Node *temp = node1;
    node1 = node2;
    node2 = temp;
}