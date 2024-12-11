#include "../include/hash_table.h"

void heapify_pq(Priority_Queue *pq) {
    lock(pq->mutex);
    heapify(pq, 0);
    unlock(pq->mutex);
}

Node* get_node(Hash_Table *table, const char *key) {
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

Hash_Table* create_table(size_t size) {
    Hash_Table *table = allocate(sizeof(Hash_Table), (void **)&table);

    if (table == NULL) {
        perror("Failed to allocate memory for Hash_Table");
        handle_sigint(1);
    }

    table->count = 0;
    table->size = size;
    table->buckets = callocate(table->size, sizeof(Node *), (void**)&table->buckets);
    pthread_mutex_init(&table->mutex, NULL);

    table->ttl_pq = create_pq(INIT_PQ_CAPACITY);
    table->freq_pq = create_pq(INIT_PQ_CAPACITY);
    return table;
}

void ht_insert(Hash_Table *table, const char *key, void *value) {
    lock(table->mutex);
    Node *node = get_node(table, key);

    // if key already exist, overwrite its value
    if (node != NULL) {
        node->freq += 1;
        heapify_pq(table->freq_pq);

        node->has_ttl = 0;
        delete_node_pq(table->ttl_pq, node);

        free(node->value);
        node->value = strdup((char *)value);
    }

    // no? create the node with key and value
    else {
        unsigned int index = hash(key, table->size);
        Node *new_node = allocate(sizeof(Node), (void**)&new_node);

        if (new_node == NULL) {
            perror("Failed to allocate memory for Node in Hash Table");
            handle_sigint(1);
        }

        new_node->key = strdup(key);
        new_node->value = strdup((char *)value);
        new_node->has_ttl = 0;
        new_node->freq = 0;
        new_node->next = table->buckets[index];
        push_pq(table->freq_pq, new_node);
        
        table->buckets[index] = new_node;
        table->count++;
    }

    unlock(table->mutex);
}

int is_expired(Node *node) {  
    if (!node->has_ttl) return 0;
    return time(NULL) > node->ttl;
}

int ht_delete(Hash_Table *table, const char *key) {
    if (table == NULL || key == NULL) return 0;
    lock(table->mutex);

    unsigned int index = hash(key, table->size);
    Node *node = table->buckets[index];
    Node *prev = NULL;

    while (node) {
        if (strcmp(node->key, key) != 0) {
            prev = node;
            node = node->next;
            continue;
        }

        delete_node_pq(table->ttl_pq, node);
        delete_node_pq(table->freq_pq, node);

        if (prev) {
            prev->next = node->next;
        } else {
            table->buckets[index] = node->next;
        }
        
        free(node->key);
        free(node->value);
        deallocate(node);
        node = NULL;
        table->count--;
        break;
    }

    unlock(table->mutex);
    return 1;
}

void* get(Hash_Table *table, const char *key) {
    if (table == NULL || key == NULL) return NULL;
    lock(table->mutex);

    Node *node = get_node(table, key);

    if (node == NULL) {
        unlock(table->mutex);
        return NULL;  // Key not found
    }

    // Check if the node is expired and delete if necessary
    if (is_expired(node)) {
        unlock(table->mutex);
        ht_delete(table, key);
        return NULL;
    }

    node->freq += 1;
    unlock(table->mutex);

    heapify_pq(table->freq_pq);
    return node->value;
}

// return 1 if success, 0 if key not found
int set_ttl(Hash_Table *table, const char *key, int seconds) {
    if (table == NULL || key == NULL) return 0;
    lock(table->mutex);

    Node *node = get_node(table, key);
    if (node == NULL) {
        unlock(table->mutex);
        return 0;  // Key not found
    }

    if (is_expired(node)) {
        unlock(table->mutex);
        ht_delete(table, key);
        return 0;  // Node was expired and deleted
    }

    node->has_ttl = 1;
    node->ttl = time(NULL) + seconds;
    node->freq += 1;

    // already had some ttl? heapify since we updated ttl
    if (node_exists_pq(table->ttl_pq, node)) {
        heapify_pq(table->ttl_pq);
    } else {
        // had no ttl, so create now
        push_pq(table->ttl_pq, node);
    }

    unlock(table->mutex);
    heapify_pq(table->freq_pq);
    return 1;
}

int get_ttl(Hash_Table *table, const char *key) {
    if (table == NULL || key == NULL) return -2;
    lock(table->mutex); 

    Node *node = get_node(table, key);
    if (node == NULL) {
        unlock(table->mutex);
        return -2;  // Key not found
    } 

    if (is_expired(node)) {
        unlock(table->mutex);
        ht_delete(table, key);
        return -2;  // Node was expired and deleted
    }

    if (!node->has_ttl) {
        unlock(table->mutex);
        return -1;  // Node does not have a TTL
    }

    node->freq += 1;
    unlock(table->mutex);

    heapify_pq(table->freq_pq);
    return node->ttl - time(NULL);
}

void destroy_ht(Hash_Table *table) {
    if (table == NULL) return;
    lock(table->mutex);

    // Free each bucket's linked list of nodes
    for (size_t i = 0; i < table->size; i++) {
        Node *current = table->buckets[i];
        while (current != NULL) {
            Node *next_node = current->next;
            free(current->key);
            free(current->value);
            deallocate(current);
            current = next_node;
        }
    }

    deallocate(table->buckets);

    unlock(table->mutex);
    pthread_mutex_destroy(&table->mutex);
    deallocate(table);
}

void* expiry_monitor(void *arg) {
    Hash_Table *table = (Hash_Table *)arg;
    if (table == NULL) return NULL;

    Priority_Queue *pq = table->ttl_pq;
    if (pq == NULL) return NULL;
    
    while (1) {
        sleep(EXPIRY_MONITOR_INTERVAL_SECONDS); 
        printf("-Waking expiry monitor-\n");
        lock(table->mutex);

        // Check if the top item in the TTL priority queue is expired
        while (pq->size > 0 && is_expired(peek_pq(pq))) {
            Node *expired_node = pop_pq(pq);
            if (expired_node != NULL) {
                delete_node_pq(table->freq_pq, expired_node);
                ht_delete(table, expired_node->key);
            }
        }

        printf("-Sleeping expiry monitor-\n");
        unlock(table->mutex);
    }

    return NULL;
}
