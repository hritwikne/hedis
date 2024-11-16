#include "../include/context.h"

static Context ctx;

Context* get_context() {
    return &ctx;
}