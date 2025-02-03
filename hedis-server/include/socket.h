#ifndef SOCKET_H
#define SOCKET_H

#include "global_includes.h"

int create_socket();
void set_socket_options(int server_fd);
void bind_socket(int server_fd, struct sockaddr_in *address, int port);
void listen_for_connections(int server_fd);

#endif
