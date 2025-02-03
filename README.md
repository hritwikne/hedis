# Hedis - A Redis Clone

## Overview
This project is a custom implementation of Redis, built entirely from scratch in C. It provides a subset of the original Redis features, built from ground up for educational purpose.

[Link to demo video of the application](https://drive.google.com/file/d/1pCi74EVRYATfrDMu5ZdxWHYO9VReyuxQ/view?usp=sharing)

## Features
* In-memory key-value store
* Built on top of TCP connection, so server and client can be in different locations
* A custom memory allocator was built from scratch in C to prevent memory leak
* Event loop, to service connection and message requests effectively without conflicts
* Set time-to-live (TTL) for data for auto deletion
* When memory is 100% utilized, least frequently used data is replaced with the new data
* Main commands: GET, SET, DEL, EXPIRE, INCR, DECR, TTL, MEMSTATS
* Publish/Subscribe functionality **(Coming soon)**
* AOF Disk Persistence **(Coming soon)**

## Key Highlights
**Multi-threaded Application Architecture**  

The application utilizes a multi-threaded design with specialized threads:

1. Main Thread
    * Primary application initialization
    * Overall coordination and control flow

2. Event Loop Thread
    * Handles client connections and message processing
    * Single-threaded, non-blocking I/O management
    * Services multiple client requests concurrently

3. Memory Compaction Thread
    * Prevents memory fragmentation
    * Dynamically optimizes memory allocation
    * Periodically reorganizes memory blocks

4. Data-node Expiry Monitor Thread
    * Manages time-to-live (TTL) for data entries
    * Automatically handles data lifecycle management
    * Removes expired data entries

While the event loop operates single-threaded, the overall application leverages multiple threads for efficient background processing and resource management.

**A Custom Memory Allocator**

Path to that file: `hedis-server/utils/mem_utils.c`

A simple abstraction of how a process looks like in memory:
```
+-------------------+
|      Stack        | Dynamic allocation
|                   | Grows downward
+-------------------+
|       Heap        | Dynamic memory
|                   | Allocations/deallocations
+-------------------+
|   BSS Segment     | Uninitialized global/static vars
+-------------------+
|  Data Segment     | Initialized global/static vars
+-------------------+
| Text/Code Segment | Read-only executable instructions
+-------------------+
```

When using C's dynamic memory allocation function `malloc()`, memory is allocated from the process heap segment. If allocated memory is not explicitly freed before process termination, this creates a memory leak - the memory remains allocated but inaccessible, unavailable to other processes until system reboot.

In contrast, memory allocated in the data segment is managed differently. The data segment is pre-allocated at process startup and automatically deallocated by the operating system when the process terminates.

I've implemented a custom memory allocator that leverages this behavior by pre-allocating a fixed-size memory pool in the data segment. This pool is partitioned into variable-sized blocks as needed during runtime, similar to how a traditional heap allocator works. The key advantage is that even if individual blocks aren't explicitly freed during execution, the entire memory pool is automatically reclaimed when the process exits.

The pool size is configurable via `MEM_POOL_SIZE_BYTES` in `hedis-server/include/constants.h`. This value determines the total memory pre-allocated in the data segment during process initialization.

But, memory fragmentation is a critical challenge in dynamic memory management. In custom memory allocators, repeated allocation and deallocation of variable-sized memory blocks can lead to a non-contiguous memory layout where sufficient total free memory exists but free memory blocks are non-adjacent and individual free blocks are insufficiently large for new allocations.

This scenario creates memory fragmentation, reducing allocation efficiency.
My implementation addresses this using a compaction algorithm:

* Periodically triggers a memory reorganization thread
* Consolidates memory blocks by moving used blocks to one side and free blocks to the other side of the memory pool
* Aggregates free blocks into a contiguous memory region

By implementing this strategy, the memory allocator maintains optimal memory utilization and mitigates fragmentation-related allocation challenges.

## Prerequisites
1. Linux environment
2. GCC compiler

## Installation
You can skip this step and go straight to the next section if you don't want to compile the program yourself.

1. Compile server app:
    ```
    cd hedis-server
    make
    ```
2. Compile client app:
    ```
    cd hedis-cli
    make
    ```
Compilation generates executables hedis-server and hedis-cli in respective bin/ directories.

## Usage:
1. Start server:
    ```
    cd hedis-server/bin
    ./hedis-server
    ```
2. Start client:
    ```
    cd hedis-cli/bin
    ./hedis-cli [HOST] [PORT]
    ```
where HOST and PORT are the address and port number of the server respectively.  
Default host: `127.0.0.1` and Deafult port: `3000`.  

**Note:** If no host or port specified, client connects to `127.0.0.1:3000`

Example usage:
```
./hedis-cli  // valid - connects to 127.0.0.1:3000
./hedis-cli 192.168.1.100 6379  // valid - connects to 192.168.1.100:6379
```

## Acknowledgements
* Redis documentation

## Limitations
This is an educational project and does not include all features of the original Redis. Use for educational purposes only.