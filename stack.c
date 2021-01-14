#include "stack.h"

typedef struct element_s
{
    void *data;
    struct element_s *next;
} Element;

typedef struct stack_s
{
    Element *head;
    int size;
} Stack;

Stack *stack_create()
{
    Stack *stack = malloc(sizeof(Stack));
    stack->head = NULL;
    stack->size = 0;

    return stack;
}

void stack_destroy(Stack **stack)
{
    if(*stack)
    {
        Element *e = (*stack)->head;

        while(e)
        {
            Element *del = e;
            e = e->next;

            free(del);
        }

        free(*stack);
        *stack = NULL;
    }
}

void *stack_top(Stack *stack)
{
    if(stack && stack->head)
        return stack->head->data;
    else
        return NULL;
}

void stack_push(Stack *stack, void *data)
{
    if(stack)
    {
        Element *new = malloc(sizeof(Element));

        new->data = data;
        new->next = stack->head;
        stack->head = new;
        stack->size++;
    }
}

void stack_pop(Stack *stack)
{
    if(stack && stack->head)
    {
        Element *del = stack->head;
        stack->head = stack->head->next;
        stack->size--;
        
        free(del);
    }
}

int stack_size(Stack *stack)
{
    if(stack)
        return stack->size;
    else
        return -1;
}
