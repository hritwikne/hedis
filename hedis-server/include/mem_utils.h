#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include "utility.h"
#include "context.h"
#include "constants.h"
#include "global_includes.h"

typedef struct Block {
    size_t size;
    struct Block *next;
    void **caller_ptr;
    int in_use;
} Block;

void* compact_memory();
void cleanup_allocator();
void init_mem_allocator();
void deallocate(void *ptr);
void* allocate(size_t size, void** caller_ptr);
void* callocate(size_t num, size_t size, void** caller_ptr);

#endif