#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#include "../include/stack.h"
#include "../include/server.h"
#include "../include/thread_pool.h"
#include "../include/client_handler.h"

#define MAX_CLIENTS 5

int server_fd;
int client_sockets[MAX_CLIENTS];

Stack* stack; // contains available client_no.s
thread_pool_t pool; 
task_args_t* args;

int addrlen;
struct sockaddr_in address;

void handle_sigint(int sig) {
    printf("\nInterrupt signal received.\nCleaning up..\n");
    thread_pool_destroy(&pool);

    for (int i=0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != -1) {
            close(client_sockets[i]);
            client_sockets[i] = -1;
        }
    }

    close(server_fd);
    free(args);
    free_stack(stack);

    exit(EXIT_SUCCESS);
}

void socket_init() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_fd == -1) {
        perror("Creating socket failed");
        exit(EXIT_FAILURE);
    }
}

void bind_and_listen(int port) {
    int opt = 1;
    addrlen = sizeof(address);

    // set it so that we can bind to an address that's already in use
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setting socket option failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // binding socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // listening for connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("\nHEDIS\nVersion: 0\tPort: %d\nListening for connections...\n", port);
}

void accept_connections() {
    int client_no;

    while (true) {
        client_no = pop(stack); // blocking code (if stack is empty)

        client_sockets[client_no] = accept(
            server_fd,
            (struct sockaddr *)&address, 
            (socklen_t *)&addrlen
        );

        if (client_sockets[client_no] < 0) {
            perror("accepting failed");
            continue;
        }

        args = malloc(sizeof(task_args_t));
        args->client_no = client_no;
        args->client_sockets = client_sockets;
        args->stack = stack;

        // add the client handling to tasks queue of thread pool 
        // where one of the thread will pick it up for execution
        thread_pool_add_task(&pool, handle_client, args);
    }
}

void start_server(int port) {
    signal(SIGINT, handle_sigint);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = -1;
    }
    
    stack = create_stack();

    thread_pool_init(&pool);
    stack_init(stack, MAX_CLIENTS);
    
    socket_init();
    bind_and_listen(port);
    accept_connections();
}
