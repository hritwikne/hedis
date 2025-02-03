#ifndef SIG_HANDLER_H
#define SIG_HANDLER_H

#include "context.h"
#include "hash_table.h"
#include "priority_queue.h"
#include "global_includes.h"

void handle_sigint(int sig);
void handle_sigsegv(int sig);

#endif