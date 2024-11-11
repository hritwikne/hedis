#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/constants.h"
#include "../include/hash_table.h"
#include "../include/priority_queue.h"

static Priority_Queue *ttl_pq;
static Priority_Queue *freq_pq;

// djb2 algorithm
unsigned int hash(const char *key, size_t size) {
    int c;
    unsigned long hash = 5381;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash + c); // in effect: (hash * 33) + c;
    }
    
    return hash % size;
}

Hash_Table* create_table(size_t size) {
    Hash_Table *table = malloc(sizeof(Hash_Table));

    if (table == NULL) {
        perror("Failed to allocate memory for hash table");
        exit(EXIT_FAILURE);
    }

    table->size = size;
    table->count = 0;
    table->buckets = calloc(table->size, sizeof(Node *));

    ttl_pq = create_pq(INIT_PQ_CAPACITY);
    freq_pq = create_pq(INIT_PQ_CAPACITY);
    return table;
}

void insert(Hash_Table *table, const char *key, void *value) {
    // if key already exist, overwrite its value
    Node *node = get_node(table, key);
    if (node != NULL) {
        node->freq += 1;
        heapify(freq_pq, 0);

        node->has_ttl = 0;
        delete_node_from_pq(ttl_pq, node);

        node->value = value;
        return;
    }

    unsigned int index = hash(key, table->size);
    Node *new_node = malloc(sizeof(Node));

    if (new_node == NULL) {
        perror("Failed to allocate memory for Node in Hash Table");
        exit(EXIT_FAILURE);
    }

    new_node->key = strdup(key);
    new_node->value = value;
    new_node->has_ttl = 0;
    new_node->freq = 0;
    new_node->next = table->buckets[index];

    push(freq_pq, new_node);
    
    table->buckets[index] = new_node;
    table->count++;
}

Node *get_node(Hash_Table *table, const char *key) {
    unsigned int index = hash(key, table->size);
    Node *node = table->buckets[index];

    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

int is_expired(Node *node) {  
    if (!node->has_ttl) return 0;
    return time(NULL) > node->ttl;
}

void delete(Hash_Table *table, const char *key) {
    unsigned int index = hash(key, table->size);
    Node *node = table->buckets[index];
    Node *prev = NULL;

    while (node) {
        if (strcmp(node->key, key) != 0) {
            prev = node;
            node = node->next;
            continue;
        }

        if (prev) {
            prev->next = node->next;
        } else {
            table->buckets[index] = node->next;
        }

        free(node->key);
        free(node);
        table->count--;
        return;
    }
}

void *get(Hash_Table *table, const char *key) {
    Node *node = get_node(table, key);

    if (node == NULL) {
        return NULL;
    } 

    if (is_expired(node)) {
        delete(table, key);
        return NULL;
    }
    
    node->freq += 1;
    heapify(freq_pq, 0);

    return node->value;
}

// return 0 if success, -1 if key not found
int set_ttl(Hash_Table *table, const char *key, int seconds) {
    Node *node = get_node(table, key);

    if (node == NULL) {
        return -1;
    }

    if (is_expired(node)) {
        delete(table, key);
    }

    node->has_ttl = 1;
    node->ttl = time(NULL) + seconds;
    push(ttl_pq, node);
    
    node->freq += 1;
    heapify(freq_pq, 0);

    return 0;
}

int get_ttl(Hash_Table *table, const char *key) {
    Node *node = get_node(table, key);

    if (node == NULL) {
        return -2;
    } else if (!node->has_ttl) {
        return -1;
    } else {
        node->freq += 1;
        heapify(freq_pq, 0);
        return node->ttl;
    }
}