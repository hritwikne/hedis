#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/context.h"
#include "../include/mem_utils.h"

static Block *free_list_head = NULL; 
static char memory_pool[MEM_POOL_SIZE_MB]; // char = untyped, represent single byte of memory

static pthread_mutex_t mem_pool_mutex = PTHREAD_MUTEX_INITIALIZER;
// PTHREAD_MUTEX_INITIALIZER is a macro that initializes the mutex at compile-time, 
// so it’s ready for use as soon as the program starts.

void init_allocator() {
    free_list_head = (Block*)memory_pool;
    free_list_head->size = (MEM_POOL_SIZE_MB - sizeof(Block));
    free_list_head->next = NULL;
}

// Align size to the nearest multiple of 8 bytes
size_t align(size_t size) {
    return (size + 7) & ~7;
}

// given pointer to a block and required space from it,
// convert remaining space to a new block
Block *split_block(Block *block, size_t size) {
    Block *new_block = (Block*)((char*)block + size);
    new_block->size = block->size - size;
    new_block->next = block->next;
    return new_block;
}

void merge_adjacent_free_blocks() {
    Block *block = free_list_head;

    while (block && block->next) {
        char *curr_block_end = (char*)block + sizeof(Block) + block->size;
        char *next_block_start = (char*)block->next;

        if (curr_block_end == next_block_start) {
            block->size += sizeof(Block) + block->next->size;
            block->next = block->next->next;
        } else {
            block = block->next;
        }
    }
}

void *allocate(size_t size) {
    size = align(size);
    pthread_mutex_lock(&mem_pool_mutex);
    
    Block *block = free_list_head;
    Block *prev_block = NULL;

    while (block) {
        // if current block isn't large enough, check next block
        if (block->size < size) {
            prev_block = block;
            block = block->next;
            continue;
        }

        // is there gonna be leftover space to store at least Block header?
        if (block->size > (size + sizeof(Block))) {
            Block *new_block = split_block(&block, size + sizeof(Block));

            block->size = size;
            block->next = new_block;
        }

        // no? then use the entire block
        else {
            if (!prev_block) {
                free_list_head = block->next; // it's the first block, so move head
            } else {
                prev_block->next = block->next; // remove current block from free_list
            }
        }

        // return pointer to the usable memory region, skipping metadata part
        return (char*)block + sizeof(Block);
    }

    // no suitable block found
    pthread_mutex_unlock(&mem_pool_mutex);
    return NULL;
}

void deallocate(void *ptr) {
    if (!ptr) return;

    pthread_mutex_lock(&mem_pool_mutex);

    Block *block = (Block*)((char*)ptr - sizeof(Block));
    block->next = free_list_head;
    free_list_head = block->next;

    merge_adjacent_free_blocks();
    pthread_mutex_unlock(&mem_pool_mutex);
}

void *compact_memory(void *arg) {
    while (1) {
        sleep(COMPACTION_INTERVAL_SECONDS);
        
        pthread_mutex_lock(&mem_pool_mutex);

        char *current = memory_pool; // init to start of mem pool
        Block *free_block = free_list_head; // start of free_list

        // move all free blocks to start of mem pool
        while (free_block) {
            if ((char*)free_block != current) {
                memmove(current, free_block, sizeof(Block) + free_block->size);
                free_block = (Block*)current;
            }

            current += (sizeof(Block) + free_block->size);
            free_block = free_block->next;
        }

        free_list_head = memory_pool;
        merge_adjacent_free_blocks();

        pthread_mutex_unlock(&mem_pool_mutex);
    }

    return NULL;
}

void cleanup_allocator() {
    pthread_mutex_destroy(&mem_pool_mutex);
}