#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <netinet/in.h>

#include "../include/stack.h"

#define BUFFER_SIZE 1024

void accept_messages(int client_fd) {
    char buffer[1024] = {0};

    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    
    // null terminate buffer when return (\r) is encountered
    buffer[strcspn(buffer, "\r")] = 0; 

    if (bytes_read < 0 || strcmp(buffer, "quit") == 0) {
        printf("Client %d disconnected.\n", client_fd);
        close(client_fd);
        return;
    }

    printf("Client %d: %s\n", client_fd, buffer);
    
    char *res = "OK\n";
    send(client_fd, res, strlen(res), 0);
}





// void handle_client() {
//     int client_no = args->client_no;
//     int* client_sockets = args->client_sockets;
//     Stack* stack = args->stack;
    
//     char buffer[BUFFER_SIZE] = {0};
//     int client_fd = client_sockets[client_no];

//     printf("Client %d connected.\n", client_no);

//     char *welcome_res = "\nHEDIS v0.1\nType HELP to get a list of supported commands.\n";
//     send(client_fd, welcome_res, strlen(welcome_res), 0);

//     while (true) {
//         memset(buffer, 0, sizeof(buffer)); 
//         ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);

//         if (bytes_read <= 0) continue;

//         // null terminate buffer when return (\r) is encountered
//         buffer[strcspn(buffer, "\r")] = 0; 

//         if (strcmp(buffer, "quit") == 0) {
//             printf("Client %d disconnected.\n", client_no);
//             break;
//         }

//         printf("Client %d: %s\n", client_no, buffer);
//         char *res = "OK\n";
//         send(client_fd, res, strlen(res), 0);
//     }

//     close(client_fd);
//     client_sockets[client_no] = -1;
//     push(stack, client_no);
//     free(args);
// }