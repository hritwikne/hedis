#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utility.h"
#include "../include/constants.h"
#include "../include/hash_table.h"
#include "../include/priority_queue.h"


Hash_Table* create_table(size_t size) {
    Hash_Table *table = malloc(sizeof(Hash_Table));

    if (table == NULL) {
        perror("Failed to allocate memory for hash table");
        exit(EXIT_FAILURE);
    }

    table->count = 0;
    table->size = size;
    table->buckets = calloc(table->size, sizeof(Node *));
    pthread_mutex_init(&table->mutex, NULL);

    table->ttl_pq = create_pq(INIT_PQ_CAPACITY);
    table->freq_pq = create_pq(INIT_PQ_CAPACITY);
    return table;
}


void insert(Hash_Table *table, const char *key, void *value) {
    pthread_mutex_lock(&table->mutex);
    Node *node = get_node(table, key);

    // if key already exist, overwrite its value
    if (node != NULL) {
        node->freq += 1;
        
        pthread_mutex_lock(&table->freq_pq->mutex);
        heapify(table->freq_pq, 0);
        pthread_mutex_unlock(&table->freq_pq->mutex);

        node->has_ttl = 0;
        delete_node_pq(table->ttl_pq, node);

        node->value = value;
    }

    // no? create the node with key and value
    else {
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

        push_pq(table->freq_pq, new_node);
        
        table->buckets[index] = new_node;
        table->count++;
    }

    pthread_mutex_unlock(&table->mutex);
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

// TODO also pop node from pqs
void delete(Hash_Table *table, const char *key) {
    if (table == NULL || key == NULL) return;
    pthread_mutex_lock(&table->mutex);

    unsigned int index = hash(key, table->size);
    Node *node = table->buckets[index];
    Node *prev = NULL;

    while (node) {
        if (strcmp(node->key, key) != 0) {
            prev = node;
            node = node->next;
            continue;
        }

        // Node found; update pointers to remove it from the list
        if (prev) {
            prev->next = node->next;
        } else {
            table->buckets[index] = node->next;
        }

        free(node->key);
        free(node);
        node = NULL;
        table->count--;
        break;
    }

    pthread_mutex_unlock(&table->mutex);
    return;
}


void *get(Hash_Table *table, const char *key) {
    if (table == NULL || key == NULL) return NULL;
    pthread_mutex_lock(&table->mutex);

    Node *node = get_node(table, key);
    if (node == NULL) {
        pthread_mutex_unlock(&table->mutex);
        return NULL;  // Key not found
    }

    // Check if the node is expired and delete if necessary
    if (is_expired(node)) {
        pthread_mutex_unlock(&table->mutex);
        delete(table, key);
        return NULL;
    }

    node->freq += 1;
    pthread_mutex_unlock(&table->mutex);

    // Update the frequency priority queue
    pthread_mutex_lock(&table->freq_pq->mutex);
    heapify(table->freq_pq, 0);
    pthread_mutex_unlock(&table->freq_pq->mutex);

    return node->value;
}


// return 0 if success, -1 if key not found
int set_ttl(Hash_Table *table, const char *key, int seconds) {
    if (table == NULL || key == NULL) return -1;
    pthread_mutex_lock(&table->mutex);

    Node *node = get_node(table, key);
    if (node == NULL) {
        pthread_mutex_unlock(&table->mutex);
        return -1;  // Key not found
    }

    if (is_expired(node)) {
        delete(table, key);
        pthread_mutex_unlock(&table->mutex);
        return -1;  // Node was expired and deleted
    }

    node->has_ttl = 1;
    node->ttl = time(NULL) + seconds;
    node->freq += 1;

    push_pq(table->ttl_pq, node);
    pthread_mutex_unlock(&table->mutex);

    pthread_mutex_lock(&table->freq_pq->mutex);
    heapify(table->freq_pq, 0);
    pthread_mutex_unlock(&table->freq_pq->mutex);

    return 0;
}


int get_ttl(Hash_Table *table, const char *key) {
    if (table == NULL || key == NULL) return -2;
    pthread_mutex_lock(&table->mutex); 

    Node *node = get_node(table, key);
    if (node == NULL) {
        pthread_mutex_unlock(&table->mutex);
        return -2;  // Key not found
    } 

    if (!node->has_ttl) {
        pthread_mutex_unlock(&table->mutex);
        return -1;  // Node does not have a TTL
    }

    node->freq += 1;
    pthread_mutex_unlock(&table->mutex);

    pthread_mutex_lock(&table->freq_pq->mutex);
    heapify(table->freq_pq, 0);
    pthread_mutex_unlock(&table->freq_pq->mutex);

    return node->ttl;
}


void destroy_ht(Hash_Table *table) {
    if (table == NULL) return;
    pthread_mutex_lock(&table->mutex);

    // Free each bucket's linked list of nodes
    for (size_t i = 0; i < table->size; i++) {
        Node *current = table->buckets[i];
        while (current != NULL) {
            Node *next_node = current->next;
            free(current->key);
            free(current);
            current = next_node;
        }
    }

    free(table->buckets);

    pthread_mutex_unlock(&table->mutex);
    pthread_mutex_destroy(&table->mutex);
    free(table);
}


void* expiry_monitor(void *arg) {
    Hash_Table *table = (Hash_Table *)arg;
    Priority_Queue *pq = table->ttl_pq;
    
    while (1) {
        sleep(EXPIRY_MONITOR_INTERVAL_SECONDS);
 
        while (is_expired(peek_pq(pq))) {
            Node *root = pop_pq(pq);
            delete(table, root->key);
        }
    }
}

void* expiry_monitor(void *arg) {
    Hash_Table *table = (Hash_Table *)arg;
    if (table == NULL) return NULL;

    Priority_Queue *pq = table->ttl_pq;
    if (pq == NULL) return NULL;
    
    while (1) {
        sleep(EXPIRY_MONITOR_INTERVAL_SECONDS); 
        pthread_mutex_lock(&table->mutex);

        // Check if the top item in the TTL priority queue is expired
        while (pq->size > 0 && is_expired(peek_pq(pq))) {
            Node *root = pop_pq(pq);
            if (root != NULL) delete(table, root->key);
        }

        pthread_mutex_unlock(&table->mutex);
    }

    return NULL;
}
