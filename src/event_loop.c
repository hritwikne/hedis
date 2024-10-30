#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/epoll.h>

#include "../include/event_loop.h"
#include "../include/msg_handler.h"
#include "../include/conn_handler.h"

/**
 * Sets up an epoll instance and configures it to monitor incoming events
 * on the specified server file descriptor (`server_fd`). It creates an epoll instance,
 * assigns it to `epoll_fd`, and adds the `server_fd` to the epoll instance, specifying
 * it as an event source for `EPOLLIN` (incoming data readiness).
 *
 * @param server_fd   The file descriptor of the server socket to be monitored for events.
 * @param epoll_fd    The file descriptor for the new epoll instance. This will be updated
 *                    within the function if creation is successful.
 * @param event       A pointer to an `epoll_event` structure that holds event details,
 *                    such as the type of event (`EPOLLIN`) and the file descriptor to monitor.
 *
 * @note              Exits the program if `epoll_create1` or `epoll_ctl` fails, printing
 *                    an error message to `stderr`.
 */
void initialize_epoll_server(int server_fd, int *epoll_fd_ptr, struct epoll_event *event) {
    int epoll_fd = epoll_create1(0);

    if (epoll_fd < 0) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    *epoll_fd_ptr = epoll_fd;

    event->events = EPOLLIN; // for incoming events (like read)
    event->data.fd = server_fd;

    int epoll_ctl_res = epoll_ctl(
        epoll_fd,
        EPOLL_CTL_ADD,
        server_fd,
        event
    );

    if (epoll_ctl_res < 0) {
        perror("Failed to add server fd to epoll instance.");
        exit(EXIT_FAILURE);
    }
}


/**
 * Runs the main event loop for handling server and client connections.
 *
 * This function continuously waits for events on an epoll instance and handles
 * each event as it occurs. If the event corresponds to the server file descriptor,
 * it calls `accept_connections` to accept a new client connection. If the event
 * is associated with a client file descriptor, it calls `accept_messages` to
 * process incoming messages from that client.
 *
 * @param args A pointer to an `EventLoopArgs` structure containing:
 *             - `epoll_fd`: The epoll instance file descriptor.
 *             - `events`: An array of `epoll_event` structures to hold triggered events.
 *             - `max_events`: The maximum number of events to handle at once.
 *             - `server_fd`: The file descriptor of the server socket.
 *             - `address`: Server address structure for connection acceptance.
 *             - `addrlen`: Length of the server address structure.
 *
 * @note This function enters an infinite loop. Ensure it has a termination signal 
 *       or condition in a real-world application. This example does not handle 
 *       errors in `epoll_wait`, `accept_connections`, or `accept_messages`, 
 *       which may require handling for production-level robustness.
 *
 * @note Frees the `args` structure at the end, but given the infinite loop, this
 *       cleanup may not be reached unless the loop is externally interrupted.
 */
void *run_event_loop(void *arg) {
    EventLoopArgs *args = (EventLoopArgs *)arg;

    while (true) {
        int num_events = epoll_wait(
            args->epoll_fd, 
            args->events, 
            args->max_events, 
            5000
        );

        if (num_events < 0) {
            perror("epoll_wait failed");
            free(args);
            exit(EXIT_FAILURE);
        } 
        
        else if (num_events == 0 && *args->terminate_sig == 1) {
            free(args);
            break;
        }

        for (int i = 0; i < num_events; i++) {
            if (args->events[i].data.fd == args->server_fd) {
                accept_connections(
                    args->server_fd,
                    args->epoll_fd,
                    args->address,
                    args->addrlen
                );
            } else {
                int client_fd = args->events[i].data.fd;
                accept_messages(client_fd);
            }
        }
    }
}