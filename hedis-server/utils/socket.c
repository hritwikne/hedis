#include "../include/socket.h"

int create_socket() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) {
        perror("Creating socket failed");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

void set_socket_options(int server_fd) {
    int opt = 1;
    
    // allow rebinding to an address in use
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setting socket option failed");
        exit(EXIT_FAILURE);
    }
}

void bind_socket(int server_fd, struct sockaddr_in *address, int port) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }
}

void listen_for_connections(int server_fd) {
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}
