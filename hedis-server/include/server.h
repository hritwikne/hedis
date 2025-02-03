#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include "context.h"
#include "utility.h"
#include "mem_utils.h"
#include "event_loop.h"
#include "hash_table.h"
#include "sig_handler.h"
#include "global_includes.h"

void start_server(int port);

#endif