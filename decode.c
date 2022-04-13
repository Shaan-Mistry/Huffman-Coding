#include "defines.h"
#include "huffman.h"
#include "io.h"
#include "header.h"
#include "node.h"
#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

/* Display  program synopsis and usage. */
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   A Huffman decoder.\n"
        "   Decompresses a file using the Huffman coding algorithm.\n"
        "\n"
        "USAGE\n"
        "   %s [-h] [-i infile] [-o outfile]\n"
        "\n"
        "OPTIONS\n"
        "   -h              Program usage and help.\n"
        "   -v              Print compression statistics.\n"
        "   -i infile       Input file to decompress.\n"
        "   -o outfile      Output of decompressed data.\n",
        exec);
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

    /* Read in header. */
    uint8_t *buf_header = (uint8_t *) malloc(sizeof(Header));
    read_bytes(fd_infile, buf_header, sizeof(Header));

    /* Check if header was read successfully. */
    if (bytes_read < sizeof(Header)) {
        fprintf(stderr, "Error: Unable to read header.\n");
        return 1;
    }

    /* Cast header buffer to a Header struct. */
    Header *h = (Header *) buf_header;

    /* Verify the magic number. */
    if (h->magic != MAGIC) {
        fprintf(stderr, "Invalid magic number.\n");
        return 1;
    }

    /* Set permissions of the outfile to match the permissions in header. */
    fchmod(fd_outfile, h->permissions);

    /* Read the dumped tree. */
    uint8_t *buf_tree = (uint8_t *) malloc(h->tree_size);
    bytes_read = 0;

    read_bytes(fd_infile, buf_tree, h->tree_size);

    /* Reconstruct Huffman tree. */
    Node *root = rebuild_tree(h->tree_size, buf_tree);

    /* Read input file one bit at a time and traverse down tree. */
    uint8_t bit;
    Node *n = root;
    uint64_t index = 0;
    bytes_read = 0;
    uint64_t bytes = 0;

    while (read_bit(fd_infile, &bit) && index < h->file_size) {
        if (!n->left && !n->right) {
            write_bytes(fd_outfile, &n->symbol, 1);
            n = root;
            index += 1;
            bytes_written = 0;
        }
        if (bit == 0) {
            n = n->left;
        }
        if (bit == 1) {
            n = n->right;
        }
        bytes += 1;
    }

    /* Print out statistics. */
    if (stats) {
        fprintf(stderr, "Compressed file size: %lu bytes\n", h->file_size);
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes);
        fprintf(stderr, "Space saving: %f %%\n", 100 * (1 - (double) bytes / h->file_size));
    }

    /* Free allocated memory. */
    free(buf_header);
    free(buf_tree);
    delete_tree(&root);

    /* Close input file and output file. */
    fclose(infile);
    fclose(outfile);

    return 0;
}
