#include "../include/resp.h"
#include "../include/msg_handler.h"

void unescape_buffer(char *buffer) {
    char *src = buffer, *dst = buffer;

    while (*src) {
        if (*src == '\\' && (*(src + 1) == 'r' || *(src + 1) == 'n')) {
            if (*(src + 1) == 'r') {
                *dst = '\r';
            } else if (*(src + 1) == 'n') {
                *dst = '\n';
            }
            src += 2; // Skip the '\\r' or '\\n'
        } else {
            *dst = *src;
            src++;
        }
        dst++;
    }
    *dst = '\0'; // Null-terminate the cleaned string
}

void accept_messages(int client_fd) {
    char buffer[1024] = {0};
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        if (bytes_read < 0) perror("Read error");
        close(client_fd);
        printf("Client %d disconnected.\n", client_fd);
        return;
    }
    
    buffer[bytes_read] = '\0';
    unescape_buffer(buffer);

    if (strcmp(buffer, "quit") == 0) {
        printf("Client %d requested to quit, so disconnecting.\n", client_fd);
        close(client_fd);
        return;
    }

    // printf("Client %d: %s\n", client_fd, buffer);
    char *res = parse_resp(buffer);

    send(client_fd, res, strlen(res), 0);
    return;
}
