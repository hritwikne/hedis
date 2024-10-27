#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>

#include "../include/thread_pool.h"
#include "../include/stack.h"

#define BUFFER_SIZE 1024

// TODO - free args in case of server interrupt
void handle_client(task_args_t* args) {
    int client_no = args->client_no;
    int* client_sockets = args->client_sockets;
    Stack* stack = args->stack;
    
    char buffer[BUFFER_SIZE] = {0};
    int client_fd = client_sockets[client_no];

    printf("Client %d connected.\n", client_no);

    char *welcome_res = "\nHEDIS v0\nType HELP to get a list of supported commands.\n";
    send(client_fd, welcome_res, strlen(welcome_res), 0);

    while (true) {
        memset(buffer, 0, sizeof(buffer)); 
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) continue;
        buffer[bytes_read - 2] = '\0'; // null terminating buffer, ignoring the \r \n in the end

        if (strcmp(buffer, "quit") == 0) {
            printf("Client %d disconnected.\n", client_no);
            break;
        }

        printf("Client %d: %s\n", client_no, buffer);
        char *res = "OK\n";
        send(client_fd, res, strlen(res), 0);
    }

    close(client_fd);
    client_sockets[client_no] = -1;
    push(stack, client_no);
    free(args);
}