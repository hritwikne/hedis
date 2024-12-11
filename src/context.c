#include "../include/context.h"

static Context ctx;
static MemoryStats mem_stats;

Context* get_context() {
    return &ctx;
}

MemoryStats* get_mem_stats() {
    return &mem_stats;
}