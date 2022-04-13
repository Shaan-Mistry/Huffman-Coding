#include "node.h"
#include "pq.h"

#include <stdio.h>
#include <stdlib.h>

/* Define the Priority Queue Struct. */
struct PriorityQueue {
    uint32_t tail;
    uint32_t capacity;
    Node **nodes;
};

/* Construct a new Priority Queue. */
PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    if (q) {
        q->tail = 0;
        q->capacity = capacity;
        q->nodes = (Node **) calloc(capacity, sizeof(Node));
        if (q->nodes) {
            return q;
        }
        free(q);
    }
    return NULL;
}

/* Delete the inputted Priority Queue. */
void pq_delete(PriorityQueue **q) {
    if (*q) {
        free((*q)->nodes);
        free(*q);
        *q = NULL;
    }
}
/* Check if the inputted Priority Queue is empty. */
bool pq_empty(PriorityQueue *q) {
    return q->tail == 0;
}

/* Check if the inputted Priority Queue is full. */
bool pq_full(PriorityQueue *q) {
    return q->tail == q->capacity;
}

/* Check the size of the inputted Priority Queue. */
uint32_t pq_size(PriorityQueue *q) {
    return q->tail;
}

/* Rearrange the nodes of the heap to fix the heap. */
void fix_heap(PriorityQueue *q, uint32_t i) {

    if (pq_size(q) < 2) {
        return;
    }
    uint32_t min = i;
    uint32_t left = 2 * i;
    uint32_t right = 2 * i + 1;

    /* Compare left child. */
    if (left < pq_size(q) && q->nodes[left]->frequency < q->nodes[min]->frequency) {
        min = left;
    }
    /* Compare right child. */
    if (right < pq_size(q) && q->nodes[right]->frequency < q->nodes[min]->frequency) {
        min = right;
    }
    /* Check if a smaller node was found. */
    if (min != i) {
        /* Swap min node with smaller node. */
        Node *temp = q->nodes[i];
        q->nodes[i] = q->nodes[min];
        q->nodes[min] = temp;
        fix_heap(q, min);
    }
}

/* Find highest Priority node and place it at index 0. */
void min_heap(PriorityQueue *q, uint32_t i) {
    uint32_t parent = i / 2;

    /* Check if child node is smaller than parent node. */
    if (q->nodes[i]->frequency < q->nodes[parent]->frequency) {
        /* Swap child node and parent node. */
        Node *temp = q->nodes[parent];
        q->nodes[parent] = q->nodes[i];
        q->nodes[i] = temp;

        /* Check if parent node was the root. */
        if (parent != 0) {
            min_heap(q, parent);
        }
    }
}

/* Enqueue a node into the Priority Queue. */
bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q)) {
        return false;
    }
    q->nodes[q->tail] = n;

    if (!pq_empty(q)) {
        min_heap(q, q->tail);
    }
    q->tail += 1;
    return true;
}

/* Dequeue highest Priority node from the Priority Queue. */
bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    }

    *n = q->nodes[0];

    q->nodes[0] = q->nodes[q->tail - 1];
    fix_heap(q, 0);
    q->tail -= 1;
    return true;
}

/* Print out the Priority Queueu. */
void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < pq_size(q); i += 1) {
        node_print(q->nodes[i]);
    }
}
