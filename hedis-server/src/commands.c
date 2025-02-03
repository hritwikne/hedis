#include "../include/commands.h"

char* execute_command(char *command, char **args, int arg_count) {
    Context *ctx = get_context();
    
    if (strcmp(command, "PING") == 0) {
        return strdup("+PONG\r\n");
    } 

    else if (strcmp(command, "HELP") == 0) {
        return strdup("$70\r\nCommands: PING, EXIT, SET, GET, DEL, EXPIRE, INCR, DECR, TTL, MEMSTATS\r\n");
    }
    
    else if (strcmp(command, "SET") == 0) {
        if (arg_count != 2) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        char *key = args[0];
        void *value = args[1];

        ht_insert(ctx->table, key, value);

        return strdup("+OK\r\n");
    } 
    
    else if (strcmp(command, "GET") == 0) {
        if (arg_count != 1) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        char *key = args[0];
        char *value = (char *) get(ctx->table, key);

        if (value) {
            char *res = NULL;
            asprintf(&res, "$%zu\r\n%s\r\n", strlen(value), value);
            return res;
        } else {
            return strdup("$-1\r\n");
        }
    }

    else if (strcmp(command, "DEL") == 0) {
        if (arg_count != 1) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        char *key = args[0];
        int res = ht_delete(ctx->table, key);

        if (res == 1) {
            return strdup(":1\r\n");
        } else {
            return strdup(":0\r\n");
        }
    }

    else if (strcmp(command, "EXPIRE") == 0) {
        if (arg_count != 2) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        char *key = args[0];
        int seconds = atoi(args[1]);

        if (seconds <= 0) {
            return strdup("-ERR invalid seconds value\r\n");
        }

        int res = set_ttl(ctx->table, key, seconds);

        if (res == 1) {
            return strdup(":1\r\n");
        } else {
            return strdup(":0\r\n");
        }
    }

    else if (strcmp(command, "TTL") == 0) {
        if (arg_count != 1) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        char *key = args[0];
        int ttl = get_ttl(ctx->table, key);

        char *res = NULL;
        asprintf(&res, ":%d\r\n", ttl);
        return res;
    }

    else if (strcmp(command, "INCR") == 0 || strcmp(command, "DECR") == 0) {
        if (arg_count != 1) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        char *key = args[0];
        char *value = (char *) get(ctx->table, key);

        if (value) {
            if (!is_valid_integer(value)) {
                return strdup("-ERR value is not an integer\r\n");
            }

            int new_val;
            if (strcmp(command, "INCR") == 0) {
                new_val = atoi(value) + 1;
            } else {
                new_val = atoi(value) - 1;
            }

            char *new_val_str = NULL;
            asprintf(&new_val_str, "%d", new_val);
            
            ht_insert(ctx->table, key, (void *) new_val_str);
            free(new_val_str);
            
            char *res = NULL;
            asprintf(&res, ":%d\r\n", new_val);
            return res;
        }

        else {
            return strdup("-ERR invalid key\r\n");
        }
    }

    else if(strcmp(command, "MEMSTATS") == 0) {
        if (arg_count > 0) {
            return strdup("-ERR wrong number of arguments for that command\r\n");
        }

        MemoryStats *mem = get_mem_stats();
        double total_mem = mem->total_memory / (1024.0); // converting bytes to KB
        double used_mem = mem->used_memory / (1024.0);
        double free_mem = mem->free_memory / (1024.0);

        char *res_str = NULL;
        char *bulk_str = NULL;

        asprintf(&res_str, "Total: %.2f KB, Used: %.2f KB, Free: %.2f KB", total_mem, used_mem, free_mem);
        asprintf(&bulk_str, "$%zu\r\n%s\r\n", strlen(res_str), res_str);

        free(res_str);
        return bulk_str;
    }

    else {
        return strdup("-ERR unknown command\r\n");
    }
}