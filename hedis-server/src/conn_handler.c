#include "../include/conn_handler.h"

void accept_connections(int server_fd, int epoll_fd, struct sockaddr_in *address, socklen_t *addrlen) {
    int client_fd = accept(server_fd, (struct sockaddr *)address, addrlen);

    if (client_fd < 0) {
        perror("accept failed");
        return;
    }

    // Set client socket to non-blocking
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    // Register new client socket with epoll for read events
    struct epoll_event client_event;
    client_event.events = EPOLLIN | EPOLLET;
    client_event.data.fd = client_fd;

    int epoll_ctl_res = epoll_ctl(
        epoll_fd, 
        EPOLL_CTL_ADD, 
        client_fd, 
        &client_event
    );

    if (epoll_ctl_res < 0) {
        perror("epoll_ctl failed to add client socket");
        close(client_fd);
    }

    printf("Client %d connected.\n", client_fd);
    
    char *welcome_res = "\nHEDIS v1.0\nType HELP to get a list of supported commands.\n";
    send(client_fd, welcome_res, strlen(welcome_res), 0);
}