#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <time.h>
#include <stdio.h>

#define INITIAL_SIZE 16
#define LOAD_FACTOR 0.75

typedef struct {
    char *key;
    void *value;
    struct Node *next; // for chaining
    
    int has_ttl;
    time_t ttl;

    int freq;
} Node;

typedef struct {
    Node **buckets;
    size_t size;
    size_t count;
} Hash_Table;

#endif