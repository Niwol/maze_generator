#ifndef STACK_H
#define STACK_H

#include <stdlib.h>

typedef struct stack_s Stack;

Stack *stack_create();
void stack_destroy(Stack **stack);
void *stack_top(Stack *stack);
void stack_push(Stack *stack, void *data);
void stack_pop(Stack *stack);
int stack_size(Stack *stack);

#endif // STACK_H