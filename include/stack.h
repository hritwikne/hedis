#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <pthread.h>

#define MAX_SIZE 10

typedef struct {
    int top;
    int items[MAX_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond;    
} Stack;

Stack* create_stack();
void stack_init(Stack* stack, int max_clients);
void push(Stack* stack, int item);
int pop(Stack* stack);
bool is_empty(Stack* stack);
void free_stack(Stack* stack);

#endif