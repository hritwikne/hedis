#include "../include/mem_utils.h"

static Block *free_list_head = NULL;
static char memory_pool[MEM_POOL_SIZE_BYTES];
static pthread_mutex_t mem_pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_memory_map() {
    return; // comment this if mem map should be printed
    char *ptr = memory_pool;
    while (ptr < memory_pool + MEM_POOL_SIZE_BYTES) {
        Block *block = (Block *)ptr;
        printf("Block at %p, size: %zu, in_use: %d, next: %p\n", 
            block, 
            block->size, 
            block->in_use, 
            block->next
        );
        ptr += sizeof(Block) + block->size;
    }
}

void init_mem_allocator() {
    free_list_head = (Block*)memory_pool;
    free_list_head->size = (MEM_POOL_SIZE_BYTES - sizeof(Block));
    free_list_head->in_use = 0;
    free_list_head->next = NULL;

    MemoryStats *mem = get_mem_stats();
    mem->free_memory = MEM_POOL_SIZE_BYTES;
    mem->total_memory = MEM_POOL_SIZE_BYTES;
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

void* allocate(size_t size, void** caller_ptr) {
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

            if (free_list_head == block) {
                free_list_head = new_block;
            }
        }

        // no? then use the entire block
        else {
            if (!prev_block) {
                free_list_head = block->next; // it's the first block, so move head
            } else {
                prev_block->next = block->next; // remove current block from free_list
            }
        }

        block->in_use = 1;
        block->caller_ptr = caller_ptr;

        size_t total_size = (size + sizeof(Block));
        MemoryStats *mem = get_mem_stats();

        mem->used_memory += total_size;
        mem->free_memory -= total_size;

        print_memory_map();
        pthread_mutex_unlock(&mem_pool_mutex);
        
        // return pointer to the usable memory region, skipping metadata part
        return (char*)block + sizeof(Block);
    }

    // no suitable block found
    print_memory_map();
    pthread_mutex_unlock(&mem_pool_mutex);
    return NULL;
}

void* callocate(size_t num, size_t size, void** caller_ptr) {
    // Calculate total size to allocate
    size_t total_size = num * size;

    // Check for overflow in multiplication
    if (num != 0 && total_size / num != size) {
        return NULL;
    }

    void* memory = allocate(total_size, caller_ptr);

    if (memory) {
        memset(memory, 0, total_size);
    }

    return memory;
}

void deallocate(void *ptr) {
    if (!ptr) return;
    pthread_mutex_lock(&mem_pool_mutex);

    Block *block = (Block*)((char*)ptr - sizeof(Block));
    size_t total_size = (block->size + sizeof(Block));

    block->in_use = 0;
    block->caller_ptr = NULL;
    block->next = free_list_head;
    free_list_head = block;

    MemoryStats *mem = get_mem_stats();
    mem->free_memory += total_size;
    mem->used_memory -= total_size;

    merge_adjacent_free_blocks();
    print_memory_map();
    pthread_mutex_unlock(&mem_pool_mutex);
}

void* compact_memory() {
    while (1) {
        sleep(COMPACTION_INTERVAL_SECONDS);
        printf("-Waking compaction-\n");
        lock(mem_pool_mutex);

        char *r = memory_pool;
        char *w = memory_pool;

        while (r < (memory_pool + 1024)) {
            Block *current = (Block *) r;
            r += sizeof(Block) + current->size;

            if (current->in_use == 1) {
                if ((char *)current != w) {
                    memmove(w, current, sizeof(Block) + current->size);
                }

                Block *new = (Block *)w;
                w += sizeof(Block) + new->size;

                *new->caller_ptr = (char *)new + sizeof(Block); 
            }
        }

        free_list_head = (Block *) w;
        free_list_head->in_use = 0;
        free_list_head->next = NULL;
        free_list_head->size = r - w - sizeof(Block);
        print_memory_map();
        unlock(mem_pool_mutex);
        printf("-Sleeping compaction-\n");
    }

    return NULL;
}

void cleanup_allocator() {
    pthread_mutex_destroy(&mem_pool_mutex);
}