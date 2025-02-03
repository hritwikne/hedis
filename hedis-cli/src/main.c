#include "../include/client.h"
#include "../include/constants.h"
#include "../include/global_includes.h"

int main(int argc, char *argv[]) {
    int port; char *host;

    if (argc == 1) {
        printf("No host or port specified as arguments.\n");
        printf("Defaulting to 'localhost' and port 3000.\n");
        host = "127.0.0.1";
        port = PORT;
    } 
    
    else if (argc != 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    else {
        host = argv[1];
        port = atoi(argv[2]);
    }

    start_client(host, port);
    return 0;
}