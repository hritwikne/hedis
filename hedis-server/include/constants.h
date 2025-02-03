#ifndef CONSTANTS_H
#define CONSTANTS_H

// Networking
#define PORT 3000
#define MAX_EVENTS 10

// Hash Table
#define HASH_TABLE_SIZE 100
#define LOAD_FACTOR 0.75

// Priority Queue
#define INIT_PQ_CAPACITY 20

// Memory
#define MEM_POOL_SIZE_BYTES 100 * 1024 // ~100KB

// Timers
#define EXPIRY_MONITOR_INTERVAL_SECONDS 1800 // 30 mins
#define COMPACTION_INTERVAL_SECONDS 3600 // 1hr

#endif
