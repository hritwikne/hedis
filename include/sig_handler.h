#ifndef SIG_HANDLER_H
#define SIG_HANDLER_H

#include "global_includes.h"
#include "context.h"

void handle_sigint(int sig);
void handle_sigsegv(int sig);
void set_context(Context *ctx);

#endif