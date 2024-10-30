#ifndef CONN_HANDLER_H
#define CONN_HANDLER_H

void accept_connections(int server_fd, int epoll_fd, struct sockaddr *address, socklen_t *addrlen);

#endif