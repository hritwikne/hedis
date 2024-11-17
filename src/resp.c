#include "../include/resp.h"

void print_command(char *command, char **args, int arg_count) {
    printf("Command: %s ", command);
    for (int i = 0; i < arg_count; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

void free_command_and_args(char *command, char **args, int arg_count) {
    free(command);
    for (int i = 0; i < arg_count; i++) {
        free(args[i]);
    }
}

char* handle_array(char* ptr) {
    char *res = NULL;

    int array_len = atoi(ptr);
    ptr = strstr(ptr, "\r\n") + 2;

    char *command = NULL;
    int arg_index = 0;
    char *args[array_len - 1];

    // extract out tokens into command and args[]
    for (int i = 0; i < array_len; i++) {
        if (*ptr != '$') continue;

        int length = atoi(ptr + 1);
        ptr = strstr(ptr, "\r\n") + 2;

        char token[length + 1];
        strncpy(token, ptr, length);
        token[length] = '\0';

        ptr += length + 2;
        if (i == 0) {
            command = strdup(token);
            to_uppercase(command);
        } else {
            args[arg_index++] = strdup(token);
        }
    }

    if (command != NULL) {
        print_command(command, args, arg_index);
        res = execute_command(command, args, arg_index);
        free_command_and_args(command, args, arg_index);
    } else {
        res = strdup("-ERR unknown error\r\n");
    }
    
    return res;
}

char* parse_resp(char *input) {
    char *ptr = input;
    char prefix = *ptr++;

    if (prefix != '*') {
        return strdup("-ERR invalid type\r\n");
    }

    return handle_array(ptr);
}
