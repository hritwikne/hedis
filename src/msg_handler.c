#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../include/msg_handler.h"

void accept_messages(int client_fd) {
    char *res = "OK\n";
    char buffer[1024] = {0};
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        if (bytes_read < 0) perror("Read error");
        close(client_fd);
        printf("Client %d disconnected.\n", client_fd);
        return;
    }
    
    // null terminate buffer when (return, new line) is encountered
    buffer[strcspn(buffer, "\r\n")] = 0;

    if (strcmp(buffer, "quit") == 0) {
        printf("Client %d requested to quit, so disconnecting.\n", client_fd);
        close(client_fd);
        return;
    }

    printf("Client %d: %s\n", client_fd, buffer);
    send(client_fd, res, strlen(res), 0);
    return;
}
