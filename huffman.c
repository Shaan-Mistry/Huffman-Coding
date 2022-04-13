#include "huffman.h"
#include "pq.h"
#include "node.h"
#include "code.h"
#include "stack.h"

#include <unistd.h>
#include <stdbool.h>

/* Code insprired by pseudcode in the asgn6.pdf document. */
Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *q = pq_create(ALPHABET);
    for (int i = 0; i < ALPHABET; i += 1) {
        if (hist[i] > 0) {
            Node *n = node_create(i, hist[i]);
            enqueue(q, n);
        }
    }
    Node *left;
    Node *right;
    Node *root;
    while (pq_size(q) > 1) {
        dequeue(q, &left);
        dequeue(q, &right);
        Node *parent = node_join(left, right);
        enqueue(q, parent);
    }
    dequeue(q, &root);
    pq_delete(&q);
    return root;
}

static Code c;
/* Code insprired by pseudcode in the asgn6.pdf document. */
void build_codes(Node *root, Code table[static ALPHABET]) {
    static bool initialized = false;

    if (!initialized) {
        c = code_init();
        initialized = true;
    }

    uint8_t bit;
    if (root) {
        if (!root->left && !root->right) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0);
            build_codes(root->left, table);
            code_pop_bit(&c, &bit);

            code_push_bit(&c, 1);
            build_codes(root->right, table);
            code_pop_bit(&c, &bit);
        }
    }
}

/* Code insprired by pseudcode in the asgn6.pdf document. */
void dump_tree(int outfile, Node *root) {
    if (root) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);

        if (!root->left && !root->right) {
            write(outfile, "L", 1);
            write(outfile, &root->symbol, 1);
        } else {
            write(outfile, "I", 1);
        }
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
    Node *root;
    Node *left;
    Node *right;

    Stack *s = stack_create(nbytes);

    for (uint16_t i = 0; i < nbytes; i += 1) {
        if (tree_dump[i] == 'I') {
            stack_pop(s, &right);
            stack_pop(s, &left);

            Node *parent = node_join(left, right);
            stack_push(s, parent);
        }
        if (tree_dump[i] == 'L') {
            Node *n = node_create(tree_dump[i + 1], 0);
            stack_push(s, n);
            i += 1;
        }
    }
    stack_pop(s, &root);
    stack_delete(&s);
    return root;
}

/* Perfoms Post-order traversal to destruct Huffman Tree. */
/* Code inspired from Dr. Long's Lecture 18 slides. */
void delete_tree(Node **root) {
    if (*root) {
        if ((*root)->left) {
            delete_tree(&(*root)->left);
        }
        if ((*root)->right) {
            delete_tree(&(*root)->right);
        }
        node_delete(root);
        *root = NULL;
    }
}
