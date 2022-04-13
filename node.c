#include "node.h"

#include <stdlib.h>
#include <stdio.h>

/* Construct a new Node. */
Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *n = (Node *) malloc(sizeof(Node));
    n->symbol = symbol;
    n->frequency = frequency;

    return n;
}

/* Delete the inputted Node. */
void node_delete(Node **n) {
    if (*n) {
        free(*n);
        *n = NULL;
    }
}

/* Join a left child Node and right child Node and return a parent Node. */
Node *node_join(Node *left, Node *right) {
    Node *n = node_create('$', left->frequency + right->frequency);

    n->left = left;
    n->right = right;

    return n;
}

/* Print out the inputted Node. */
void node_print(Node *n) {
    printf("symbol: %u, frequency: %lu\n", n->symbol, n->frequency);
}
