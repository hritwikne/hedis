#include "../include/conn_handler.h"

int connect_to_redis(char* host, int port) {
    printf("Trying to connect to %s:%d\n", host, port);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        printf("Verify if the server is running on the correct port.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected successfully.\n");
    return sockfd;
}