#include "io.h"
#include "defines.h"
#include "code.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

uint64_t bytes_read;
uint64_t bytes_written;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    int bytes = 0;
    while ((bytes = read(infile, buf + bytes_read, nbytes - bytes_read)) > 0) {
        bytes_read += bytes;
    }
    return bytes_read;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int bytes = 0;
    while ((bytes = write(outfile, buf + bytes_written, nbytes - bytes_written)) > 0) {
        bytes_written += bytes;
    }
    return bytes_written;
}

bool read_bit(int infile, uint8_t *bit) {
    static uint8_t buf[BLOCK];
    static uint32_t index = 0;
    static uint32_t end = 0;
    int bytes = 0;

    /* Code Inspired from Eugene's Lab Section 2/25/2022. */
    if (index == 0) {
        if ((bytes = read_bytes(infile, buf, BLOCK)) < BLOCK) {
            end = bytes * 8 + 1;
        }
    }
    if (index == end) {
        free(buf);
        return false;
    }
    *bit = buf[index / 8] >> index % 8 & 0x1;
    index += 1;
    return true;
}

static uint8_t buffer[BLOCK];
static uint64_t code_index = 0;
/* Code Inspired From Eugene's Lab Section. */
void write_code(int outfile, Code *c) {
    uint8_t bit;

    for (uint32_t i = 0; i < code_size(c); i += 1) {
        bit = code_get_bit(c, i);
        if (bit == 1) {
            buffer[code_index / 8] |= 0x1 << code_index % 8;
        }
        code_index += 1;
        /* Check if buffer is full */
        if (code_index == sizeof(buffer) * 8) {
            flush_codes(outfile);
        }
    }
}

void flush_codes(int outfile) {
    /* Code Inspired from Eugene's Lab Section. */
    if (code_index > 0) {
        int bytes = (code_index + 7) / 8;
        write(outfile, buffer, bytes);
        memset(buffer, 0, sizeof(buffer)); // Zeroes out the buffer
        code_index = 0;
    }
}
