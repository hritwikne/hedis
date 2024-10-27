#include <stdbool.h>
#include <stdlib.h>

#include "../include/stack.h"

Stack* create_stack() {
    Stack* stack = malloc(sizeof(Stack*));
    return stack;
}

void stack_init(Stack* stack, int max_clients) {
    stack->top = -1;
    pthread_mutex_init(&stack->mutex, NULL);
    pthread_cond_init(&stack->cond, NULL);

    // Push available client_nos
    for (int i = 0; i < max_clients; i++) {
        push(stack, i); 
    }
}

bool is_empty(Stack* stack) {
    return stack->top == -1;
}

void push(Stack* stack, int item) {
    pthread_mutex_lock(&stack->mutex);

    if (stack->top == MAX_SIZE - 1) {
        pthread_mutex_unlock(&stack->mutex);
        return;
    }

    stack->items[++stack->top] = item;
    pthread_cond_signal(&stack->cond);
    pthread_mutex_unlock(&stack->mutex);
}

int pop(Stack* stack) {
    pthread_mutex_lock(&stack->mutex);

    if (is_empty(stack)) {
        // wait until some item is pushed on to stack
        pthread_cond_wait(&stack->cond, &stack->mutex);
    }

    int item = stack->items[stack->top--];
    pthread_mutex_unlock(&stack->mutex);
    return item;
}

void free_stack(Stack* stack) {
    free(stack);
}