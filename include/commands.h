#ifndef COMMANDS_H
#define COMMANDS_H

#include "context.h"
#include "utility.h"
#include "hash_table.h"
#include "data_structures.h"
#include "global_includes.h"

char* execute_command(char *command, char **args, int arg_count);

#endif