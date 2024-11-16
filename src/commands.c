#include "../include/commands.h"

char* execute_command(char *command, char **args, int arg_count) {
    Context *ctx = get_context();
    char *res = NULL;
    
    if (strcmp(command, "PING") == 0) {
        res = "+PONG\r\n";
        return res;
    } 

    else if (strcmp(command, "HELP") == 0) {
        res = "$60\r\nCommands: PING, SET, GET, DEL, EXPIRE, INCR, DECR, TTL, MGET\r\n";
        return res;
    }
    
    else if (strcmp(command, "SET") == 0) {
        if (arg_count != 2) {
            res = "-ERR wrong number of arguments for command\r\n";
            return res;
        }

        char *key = args[0];
        void *value = args[1];
        ht_insert(ctx->table, key, value);
        res = "+OK\r\n";
        return res;
    } 
    
    else if (strcmp(command, "GET") == 0) {
        if (arg_count != 1) {
            res = "-ERR wrong number of arguments for command\r\n";
            return res;
        }

        char *key = args[0];
        void *value = get(ctx->table, key);
        if (value) {
        // TODO, change this to: asprintf(&res, "$%zu\r\n%s\r\n", strlen(value), value);
            res = (char *)value;
        }
        else res = "$-1\r\n";
        return res;
    }

    else if (strcmp(command, "DEL") == 0) {
        if (arg_count != 1) {
            res = "-ERR wrong number of arguments for command\r\n";
            return res;
        }

        char *key = args[0];
        int status = ht_delete(ctx->table, key);
        if (status == 1) res = ":1\r\n";
        else res = ":0\r\n";
        return res;
    }

    else if (strcmp(command, "EXPIRE") == 0) {
        if (arg_count != 2) {
            res = "-ERR wrong number of arguments for command\r\n";
            return res;
        }

        char *key = args[0];
        int seconds = atoi(args[1]);

        if (seconds <= 0) {
            res = "-ERR invalid seconds value\r\n";
            return res;
        }

        int status = set_ttl(ctx->table, key, seconds);
        if (status == 1) res = ":1\r\n";
        else res = ":0\r\n";
        return res;
    }

    else if (strcmp(command, "TTL") == 0) {
        if (arg_count != 1) {
            res = "-ERR wrong number of arguments for command\r\n";
            return res;
        }

        char *key = args[0];
        int ttl = get_ttl(ctx->table, key);

        if (ttl == -1) {
            res = ":-1\r\n";
        } else if (ttl == -2) {
            res = ":-2\r\n";
        } else {
            // TODO, send ttl using asprintf
            res = ":0\r\n";
        }

        return res;
    }

    else if (strcmp(command, "INCR") == 0 || strcmp(command, "DECR") == 0) {
        if (arg_count != 1) {
            res = "-ERR wrong number of arguments for command\r\n";
            return res;
        }

        char *key = args[0];
        void *value = get(ctx->table, key);

        if (value) {
            char *temp = (char *)value;

            if (!is_valid_integer(temp)) {
                res = "-ERR value is not an integer\r\n";
                return res;
            }

            int new_val;
            if (strcmp(command, "INCR") == 0) {
                new_val = atoi(temp) + 1;
            } else {
                new_val = atoi(temp) - 1;
            }

            // TODO, cast new_val
            ht_insert(ctx->table, key, new_val);
            // TODO, use asprintf() to send new_val in res
            res = ":new_val\r\n";
        }

        return res;
    }
    
    // MGET (TODO)

    else if (strcmp(command, "QUIT") == 0) {
        res = "QUIT";
        return res;
    }
    
    else {
        res = "-ERR unknown command\r\n";
        return res;
    }
}