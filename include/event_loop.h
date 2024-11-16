#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include "context.h"
#include "constants.h"
#include "global_includes.h"

void init_epoll_server();
void *run_event_loop(void *arg);

#endif