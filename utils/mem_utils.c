#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/context.h"
#include "../include/mem_utils.h"

static Block *free_list_head = NULL;
static char memory_pool[MEM_POOL_SIZE_MB];
static pthread_mutex_t mem_pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_allocator() {
    free_list_head = (Block*)memory_pool;
    free_list_head->size = (MEM_POOL_SIZE_MB - sizeof(Block));
    free_list_head->next = NULL;
}

// Align size to the nearest multiple of 8 bytes
size_t align(size_t size) {
    return (size + 7) & ~7;
}

void merge_adjacent_free_blocks() {
    Block *block = free_list_head;

    while (block && block->next) {
        char *curr_block_end = (char*)block + sizeof(Block) + block->size;
        char *next_block_start = (char*)block->next;

        if (curr_block_end == next_block_start) {
            // means they're adjacent
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

        // is there gonna be leftover space to store at least Block header? then split it
        if (block->size > (size + sizeof(Block))) {
            Block *new_block = (Block*)((char*)block + size + sizeof(Block));
            new_block->size = block->size - size - sizeof(Block);
            new_block->next = block->next;

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
    free_list_head = block;

    merge_adjacent_free_blocks();
    pthread_mutex_unlock(&mem_pool_mutex);
}

void *compact_memory(void *arg) {
    int *terminate_sig = (int*)arg;

    while (1) {
        if (*terminate_sig == 1) break;
        pthread_mutex_lock(&mem_pool_mutex);

        char *current = memory_pool;
        Block *free_block = free_list_head;

        // move all free blocks to start of mem pool
        while (free_block) {
            if ((char*)free_block != current) {
                memmove(current, free_block, sizeof(Block) + free_block->size);
                free_block = (Block*)current;
            }

            current += (sizeof(Block) + free_block->size);
            free_block = free_block->next;
        }

        free_list_head = (Block*)memory_pool;
        merge_adjacent_free_blocks();

        pthread_mutex_unlock(&mem_pool_mutex);
        sleep(COMPACTION_INTERVAL_SECONDS);
    }

    return NULL;
}

void cleanup_allocator() {
    pthread_mutex_destroy(&mem_pool_mutex);
}