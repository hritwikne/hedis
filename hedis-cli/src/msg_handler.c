#include "../include/msg_handler.h"

void send_command(int sockfd, const char *command) {
    if (send(sockfd, command, strlen(command), 0) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}

void receive_response(int sockfd) {
    char buffer[1024];
    int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received < 0) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
        printf("\nServer closed the connection for an unknown reason.\n");
        close(sockfd);
        exit(EXIT_SUCCESS);
    }

    buffer[bytes_received] = '\0';
    char *res = parse_response(buffer);
    printf("%s\n", res);
    free(res);
}