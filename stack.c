#include "stack.h"
#include "node.h"

#include <stdlib.h>
#include <stdio.h>

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node));
        if (s->items) {
            return s;
        }
        free(s);
    }
    return NULL;
}

void stack_delete(Stack **s) {
    if (s) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
}

bool stack_empty(Stack *s) {
    return s->top == 0;
}

bool stack_full(Stack *s) {
    return s->top == s->capacity;
}

uint32_t stack_size(Stack *s) {
    return s->top;
}

bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) {
        return false;
    }
    s->items[s->top] = n;
    s->top += 1;
    return true;
}

bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    }
    *n = s->items[s->top - 1];
    s->top -= 1;
    return true;
}

void stack_print(Stack *s) {

    for (uint32_t i = 0; i < stack_size(s); i += 1) {
        node_print(s->items[i]);
    }
}
