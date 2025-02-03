#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include "resp.h"
#include "global_includes.h"

void receive_response(int sockfd);
void send_command(int sockfd, const char *command);

#endif