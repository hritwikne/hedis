#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "utility.h"
#include "mem_utils.h"
#include "constants.h"
#include "sig_handler.h"
#include "priority_queue.h"
#include "global_includes.h"
#include "data_structures.h"

void* expiry_monitor(void *arg);
void destroy_ht(Hash_Table *table);
Hash_Table* create_table(size_t size);
void* get(Hash_Table *table, const char *key);
int get_ttl(Hash_Table *table, const char *key);
int ht_delete(Hash_Table *table, const char *key);
int set_ttl(Hash_Table *table, const char *key, int seconds);
void ht_insert(Hash_Table *table, const char *key, void *value);

#endif