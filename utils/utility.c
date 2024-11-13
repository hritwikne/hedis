#include <string.h>

// djb2 algorithm
unsigned int hash(const char *key, size_t size) {
    int c;
    unsigned long hash = 5381;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash + c); // in effect: (hash * 33) + c;
    }
    
    return hash % size;
}