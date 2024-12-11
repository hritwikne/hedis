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

void spawn_thread(pthread_t *thread, void *worker, void *arg) {
    int thread_creation_res = pthread_create(thread, NULL, worker, arg);
    if (thread_creation_res < 0) {
        perror("Failed to spawn a thread.");
        exit(EXIT_FAILURE);
    }
}

void to_uppercase(char *str) {
    if (str == NULL) return;

    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int is_valid_integer(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    if (*str == '-') {
        str++;
    }

    while (*str) {
        if (!isdigit(*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

char *mystrdup(const char *s) {
    char *dup = allocate(strlen(s) + 1, (void**)&dup);
    if (dup) {
        strcpy(dup, s);
    }
    return dup;
}
