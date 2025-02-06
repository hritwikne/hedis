#include "../include/server.h"
#include "../include/constants.h"

int main(int argc, char *argv[]) {
    int port;

    if (argc == 1) {
        port = PORT;
    } 
    else if (argc != 2) {
        fprintf(stderr, "Usage: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    } 
    else {
        port = atoi(argv[1]);
    }

    start_server(port);
    return 0;
}
