#include "bm.c"
#include "hamming.c"

#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#define OPTIONS "hi:o:"

void print_help(char *path);
uint8_t lower_nibble(uint8_t val);
uint8_t upper_nibble(uint8_t val);
uint8_t pack_byte(uint8_t upper, uint8_t lower);
BitMatrix *create_generator(void);

int main(int argc, char **argv) {

    FILE *infile = stdin;
    FILE *outfile = stdout;

    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': print_help(argv[0]); return 0;
        case 'i':
            infile = fopen(optarg, "r");
            // Error handling referencing piazza post @619
            if (!infile) {
                fprintf(stderr, "Error: failed to open %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            if (!outfile) {
                fprintf(stderr, "Error: failed to open %s\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        default: print_help(argv[0]); return 0;
        }
    }
    if (infile != stdin) {
	// Set outfile to have the same permissions as infile
        struct stat statbuf;
        fstat(fileno(infile), &statbuf);
        fchmod(fileno(outfile), statbuf.st_mode);
    }

    BitMatrix *G = create_generator();

    // Scan bytes, encode and write lower and upper nibbles
    int scan = 0;
    while ((scan = fgetc(infile)) != EOF) {
        uint8_t byte = scan;
        uint8_t code_lower = ham_encode(G, lower_nibble(byte));
        uint8_t code_upper = ham_encode(G, upper_nibble(byte));
        fputc(code_lower, outfile);
        fputc(code_upper, outfile);
    }

    bm_delete(&G);
    fclose(infile);
    fclose(outfile);
    return 0;
}

// Create BitMatrix and sets bits to create Hamming(8,4) generator
BitMatrix *create_generator() {
    BitMatrix *G = bm_create(4, 8);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            if (j < 4 && i == j) {
                bm_set_bit(G, i, j);
            } else if (j >= 4 && i != (j - 4)) {
                bm_set_bit(G, i, j);
            }
        }
    }
    return G;
}

// Given by Prof. Long
uint8_t lower_nibble(uint8_t val) {
    return val & 0xF;
}
uint8_t upper_nibble(uint8_t val) {
    return val >> 4;
}
uint8_t pack_byte(uint8_t upper, uint8_t lower) {
    return (upper << 4) | (lower & 0xF);
}
//---

void print_help(char *path) {
    printf("SYNOPSIS\n");
    printf("  An encoder for systematic Hamming(8, 4).\n\n");
    printf("USAGE\n");
    printf("  %s [-h] [-i infile] [-o outfile]\n\n", path);
    printf("OPTIONS\n");
    printf("  -%-14s Program usage and help.\n", "h");
    printf("  -%-14s Specify input file to encode.\n", "i input");
    printf("  -%-14s Specify output file for encoded data.\n", "o output");
}
