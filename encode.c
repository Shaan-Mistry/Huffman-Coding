#include "defines.h"
#include "huffman.h"
#include "io.h"
#include "header.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>

/* Display  program synopsis and usage. */
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   A Huffman encoder.\n"
        "   Compresses a file using the Huffman coding algorithm.\n"
        "\n"
        "USAGE\n"
        "   %s [-h] [-i infile] [-o outfile]\n"
        "\n"
        "OPTIONS\n"
        "   -h              Program usage and help.\n"
        "   -v              Print compression statistics.\n"
        "   -i infile       Input file to compress.\n"
        "   -o outfile      Output of compressed data.\n",
        exec);
}

/* Code inspired from Eugene's Lab Section. */
/* Specifically, Function hacky_mkstemp() */
int create_tmp(int infile) {
    int bytes = 0;
    uint8_t buffer[BLOCK];
    char path[1024] = { '\0' };

    // stores the path of the file
    sprintf(path, "/tmp/huffman.%d.tmp", getpid());

    int tempfile = open(path, O_CREAT | O_TRUNC | O_RDWR, 0600);
    unlink(path);
    do {
        bytes = read(infile, buffer, sizeof(buffer));
        write(tempfile, buffer, bytes);
    } while (bytes > 0);

    lseek(tempfile, 0, SEEK_SET);
    close(infile);
    infile = tempfile;
    return infile;
}

int main(int argc, char **argv) {
    FILE *infile = NULL;
    FILE *outfile = NULL;
    bool stats = false;
    int opt;

    /* Parse Command-line arguemnts. */
    while ((opt = getopt(argc, argv, "hi:o:v")) != -1) {
        switch (opt) {
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'v': stats = true; break;
        case 'h': usage(argv[0]); return 0;
        default: break;
        }
    }

    /* Set input file to stdin if none specified. */
    if (infile == NULL) {
        infile = stdin;
    }

    /* Set output file to stdout if none specified. */
    if (outfile == NULL) {
        outfile = stdout;
    }

    /* Get file descriptor of input file and output file. */
    int fd_infile = fileno(infile);
    int fd_outfile = fileno(outfile);

    if (infile == stdin) {
        fd_infile = create_tmp(fd_infile);
    }

    /* Compute a histogram of the file. */
    uint64_t histogram[ALPHABET] = { 0 };
    uint8_t *buf = (uint8_t *) calloc(BLOCK, sizeof(uint8_t));

    while (read_bytes(fd_infile, buf, BLOCK) > 0) {
        for (uint64_t i = 0; i < bytes_read; i += 1) {
            histogram[buf[i]] += 1;
        }
        bytes_read = 0;
    }

    /* Increment element 0 and element 255. */
    histogram[0] += 1;
    histogram[ALPHABET - 1] += 1;

    /* Construct a Huffman tree. */
    Node *root = build_tree(histogram);

    /* Construct a code table by traversing the Huffman tree. */
    Code table[ALPHABET] = { 0 };
    build_codes(root, table);

    /* Construct a header. */
    struct stat fstats;
    fstat(fd_infile, &fstats);

    int symbols = 0;
    for (int i = 0; i < ALPHABET; i += 1) {
        if (histogram[i] > 0) {
            symbols += 1;
        }
    }

    uint16_t tree_size = 3 * symbols - 1;

    Header h = { MAGIC, fstats.st_mode, tree_size, fstats.st_size };

    /* Set permissions of outfile to match infile permissions. */
    fchmod(fd_outfile, fstats.st_mode);

    /* Write the constructed header to the output file. */
    write_bytes(fd_outfile, (uint8_t *) &h, sizeof(Header));

    /* Write the constructed Huffman tree to the output file. */
    dump_tree(fd_outfile, root);

    /* Write the corresponding code for each symbol. */
    lseek(fd_infile, 0, SEEK_SET);

    while (read_bytes(fd_infile, buf, BLOCK) > 0) {
        for (uint64_t i = 0; i < bytes_read; i += 1) {
            write_code(fd_outfile, &table[buf[i]]);
        }
        bytes_read = 0;
    }

    flush_codes(fd_outfile);

    /* If statistics is true, print statistics. */
    if (stats) {
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", h.file_size);
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %f %%\n", 100 * (1 - bytes_written / (double) h.file_size));
    }

    /* Free allocated memory. */
    if (root) {
        delete_tree(&root);
    }
    free(buf);

    /* Close input file and output file. */
    fclose(infile);
    fclose(outfile);

    return 0;
}
