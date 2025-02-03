#include "../include/client.h"

void start_client(char *host, int port) {
    int sockfd = connect_to_redis(host, port);
    receive_response(sockfd);
    char input[128];

    while (1) {
        printf("hedis> ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';
        
        if (strlen(input) == 0) continue;
        if (strcmp(input, "exit") == 0) break;

        char *resp_command = format_resp_command(input);
        send_command(sockfd, resp_command);
        free(resp_command);

        receive_response(sockfd);
    }

    close(sockfd);
    printf("Disconnected from Hedis server.\n");
}