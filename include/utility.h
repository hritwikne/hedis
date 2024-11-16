#ifndef UTILITY_H
#define UTILITY_H

#include "data_structures.h"
#include "global_includes.h"

void spawn_thread(pthread_t thread, void *worker);
unsigned int hash(const char *key, size_t size);
void lock(pthread_mutex_t mutex);
void unlock(pthread_mutex_t mutex);
void swap_nodes(Node *node1, Node *node2);

#endif