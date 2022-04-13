#include "code.h"
#include <stdio.h>
#include <inttypes.h>

/* Create a new code struct. */
Code code_init(void) {
    uint8_t bits[MAX_CODE_SIZE];

    for (int i = 0; i < MAX_CODE_SIZE; i += 1) {
        bits[i] = 0;
    }
    Code c = { 0, { *bits } };
    return c;
}

/* Return the size of the inputted code. */
uint32_t code_size(Code *c) {
    return c->top;
}

/* Check if the inputted code is empty. */
bool code_empty(Code *c) {
    return c->top == 0;
}

/* Check if the code is full. */
bool code_full(Code *c) {
    return c->top == 256;
}

/* Set the bit at index i to 1. */
bool code_set_bit(Code *c, uint32_t i) {
    if (i > code_size(c)) {
        return false;
    }
    c->bits[i / 8] |= 0x1 << i % 8;
    return true;
}

/* Set the bit at index i to 0. */
bool code_clr_bit(Code *c, uint32_t i) {
    if (i > code_size(c)) {
        return false;
    }
    c->bits[i / 8] &= ~(0x1 << i % 8);
    return true;
}

/* Return the bit at index i. (True = 1, False = 0) */
bool code_get_bit(Code *c, uint32_t i) {
    if (i > code_size(c)) {
        return false;
    }
    return c->bits[i / 8] >> i % 8 & 0x1;
}

/* Push a bit into the code. */
bool code_push_bit(Code *c, uint8_t bit) {
    if (code_full(c)) {
        return false;
    }
    if (bit == 1) {
        code_set_bit(c, c->top);
    } else if (bit == 0) {
        code_clr_bit(c, c->top);
    } else {
        return false;
    }
    c->top += 1;
    return true;
}

/* Pop a bit from the code. */
bool code_pop_bit(Code *c, uint8_t *bit) {
    if (code_empty(c)) {
        return false;
    }
    c->top -= 1;
    *bit = code_get_bit(c, c->top) ? 1 : 0;
    return true;
}

/* Print out the inputted code. */
void code_print(Code *c) {
    printf("top: %u\n", c->top);
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i += 1) {
        printf("byte %u: ", i);
        printf("0x%02" PRIx8 "\n", c->bits[i]);
    }
}
