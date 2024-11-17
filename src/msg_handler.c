#include "../include/resp.h"
#include "../include/msg_handler.h"

void accept_messages(int client_fd) {
    char buffer[1024] = {0};
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        if (bytes_read < 0) perror("Read error");
        close(client_fd);
        printf("\nClient %d disconnected.\n", client_fd);
        return;
    }
    
    buffer[bytes_read] = '\0';

    printf("\nReceived msg from Client %d.\n", client_fd);
    char *res = parse_resp(buffer);

    if (res == NULL) {
        fprintf(stderr, "Error: Failed to parse response from client %d\n", client_fd);
        close(client_fd);
        return;
    }

    ssize_t bytes_sent = send(client_fd, res, strlen(res), 0);
    
    if (bytes_sent < 0) {
        perror("Send error");
        free(res);
        close(client_fd);
        return;
    }

    free(res);
    return;
}
