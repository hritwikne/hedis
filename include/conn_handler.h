#ifndef CONN_HANDLER_H
#define CONN_HANDLER_H

#include "global_includes.h"

void accept_connections(int server_fd, int epoll_fd, struct sockaddr_in *address, socklen_t *addrlen);

#endif