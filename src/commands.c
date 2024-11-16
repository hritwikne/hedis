#include "../include/commands.h"

char* execute_command(char *command, char **args, int arg_count) {
    Context *ctx = get_context();
    char *res = "+OK\r\n";
    
    if (strcmp(command, "PING") == 0) {
        res = "+PONG\r\n";
        return res;
    } 

    else if (strcmp(command, "HELP") == 0) {
        res = "+PING\r\n";
        return res;
    }
    
    else if (strcmp(command, "SET") == 0 && arg_count == 2) {
        char *key = args[0];
        void *value = args[1];
        ht_insert(ctx->table, key, value);
        return res;
    } 
    
    else if (strcmp(command, "GET") == 0 && arg_count == 1) {
        char *key = args[0];
        char *res1 = (char*) get(ctx->table, key);
        return res1;
    }

    // DEL
    // EXPIRE
    // INCR
    // DECR
    // MGET
    // TTL
    
    else {
        res = "-error\r\n";
        return res;
    }
}