#ifndef RESP_H
#define RESP_H

#include "global_includes.h"

char *parse_response(char *ptr);
char *format_resp_command(const char *input);

#endif