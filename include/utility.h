#ifndef UTILITY_H
#define UTILITY_H

#include "data_structures.h"
#include "global_includes.h"

void to_uppercase(char *str);
int is_valid_integer(const char *str);
void lock(pthread_mutex_t mutex);
void unlock(pthread_mutex_t mutex);
void swap_nodes(Node *node1, Node *node2);
unsigned int hash(const char *key, size_t size);
void spawn_thread(pthread_t *thread, void *worker);

#endif