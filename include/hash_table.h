#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <time.h>
#include <stdio.h>
#include <pthread.h>

struct Priority_Queue; // forward declaration

#define INITIAL_SIZE 16
#define LOAD_FACTOR 0.75

typedef struct {
    char *key;
    void *value;
    struct Node *next; // for chaining
    
    time_t ttl;
    
    int freq;
    int has_ttl;
} Node;


typedef struct {
    pthread_mutex_t mutex;
    Priority_Queue *ttl_pq;
    Priority_Queue *freq_pq;
    Node **buckets;
    size_t size;
    size_t count;
} Hash_Table;

Hash_Table* create_table(size_t size);
void insert(Hash_Table *table, const char *key, void *value);
int get_ttl(Hash_Table *table, const char *key);
int set_ttl(Hash_Table *table, const char *key, int seconds);
void *get(Hash_Table *table, const char *key);
void* expiry_monitor(void *arg);

#endif