#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include "global_includes.h"
#include "../include/context.h"
#include "../include/constants.h"

typedef struct Block {
    size_t size;
    struct Block *next;
} Block;

void init_allocator();
void *allocate(size_t size);
void deallocate(void *ptr);
void *compact_memory(void *arg);
void cleanup_allocator();

#endif