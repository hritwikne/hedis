#include "../include/resp.h"

char* handle_array(char* ptr) {
    int array_len = atoi(ptr);
    ptr = strstr(ptr, "\r\n") + 2;

    char *command = NULL;
    char *args[array_len - 1];
    int arg_index = 0;

    for (int i = 0; i < array_len; i++) {
        if (*ptr != '$') continue;

        int length = atoi(ptr + 1);
        ptr = strstr(ptr, "\r\n") + 2;


        char element[length + 1];
        strncpy(element, ptr, length);
        element[length] = '\0';

        ptr += length + 2;
        if (i == 0) {
            command = strdup(element);
        } else {
            args[arg_index++] = strdup(element);
        }
    }

    char *res;
    if (command != NULL) {
        printf("Command: %s ", command);
        for (int i = 0; i < arg_index; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
        res = execute_command(command, args, arg_index);
    }
    
    free(command);
    for (int i = 0; i < arg_index; i++) {
        free(args[i]);
    }

    return res;
}

char* handle_bulk_string(char* ptr) {
    int length = atoi(ptr);
    ptr = strstr(ptr, "\r\n") + 2;
    char bulk_string[length + 1];
    strncpy(bulk_string, ptr, length);
    bulk_string[length] = '\0';
    printf("Bulk String: %s\n", bulk_string);
}

char* handle_integer(char *ptr) {
    int value = atoi(ptr);
    printf("Integer: %d\n", value);
}

char* handle_error(char* ptr) {
    printf("Error: %s\n", ptr);
}

char* handle_string(char *ptr) {
    printf("Simple String: %s\n", ptr);
}

char* parse_resp(char *input) {
    char *ptr = input;
    char prefix = *ptr++;

    switch (prefix) {
        case '+': { 
            handle_string(ptr);
            break;
        }
        case '-': {
            handle_error(ptr);
            break;
        }
        case ':': {
            handle_integer(ptr);
            break;
        }
        case '$': {
            handle_bulk_string(ptr);
            break;
        }
        case '*': {
            return handle_array(ptr);
        }
        default: {
            char *res = "What is that?\n";
            return res;
        }
    }
}
