#include "../include/resp.h"

char* format_resp_command(const char *input) {
    int buffer_size = 1024;
    char *formatted = malloc(buffer_size);

    if (!formatted) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    char *token;
    char *input_copy = strdup(input);
    int arg_count = 0;

    // Count arguments and tokenize input
    token = strtok(input_copy, " ");
    while (token) {
        arg_count++;
        token = strtok(NULL, " ");
    }

    snprintf(formatted, buffer_size, "*%d\r\n", arg_count);

    // Tokenize input again to build RESP format
    free(input_copy);
    input_copy = strdup(input);
    token = strtok(input_copy, " ");
    while (token) {
        snprintf(
            formatted + strlen(formatted), 
            buffer_size - strlen(formatted), 
            "$%lu\r\n%s\r\n",
            strlen(token), token
        );
        token = strtok(NULL, " ");
    }

    free(input_copy);
    return formatted;
}

char *merge_strings(char *str1, char *str2) {
    char *res = malloc(strlen(str1) + strlen(str2) + 1);
    
    if (!res) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    strcpy(res, str1);
    strcat(res, str2);

    return res;
}

char* parse_response(char *buffer) {
    if (buffer[0] == '+') {
        return strdup(buffer + 1);
    } 
    else if (buffer[0] == '-') {
        char *prefix = "(error) ";
        return merge_strings(prefix, buffer + 5);
    }
    else if (buffer[0] == ':') {
        char *prefix = "(integer) ";
        return merge_strings(prefix, buffer + 1);
    } 
    else if (buffer[0] == '$') {
        int length = atoi(buffer + 1);
        if (length == -1) {
            return strdup("(nil)\n");
        }

        char *bulk_string = malloc(length + 4); 
        char *data_start = strstr(buffer, "\r\n") + 2;

        bulk_string[0] = '\"';
        strncpy(bulk_string + 1, data_start, length);
        // adding \r\n at end to look prettier in terminal (and uniform with other command exec)
        bulk_string[length + 1] = '\"'; 
        bulk_string[length + 2] = '\r'; 
        bulk_string[length + 3] = '\n';
        bulk_string[length + 4] = '\0';
        
        return bulk_string;
    } else {
        return strdup(buffer);
    }
}