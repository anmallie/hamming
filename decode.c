#include "bm.c"
#include "hamming.c"

#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

#define OPTIONS "hvi:o:"

void print_help(char *path);
uint8_t lower_nibble(uint8_t val);
uint8_t upper_nibble(uint8_t val);
uint8_t pack_byte(uint8_t upper, uint8_t lower);
BitMatrix *create_transpose(void);

static uint32_t bytes_processed = 0;
static uint32_t err_uncorrected = 0;
static uint32_t err_corrected = 0;

int main(int argc, char **argv) {

    FILE *infile = stdin;
    FILE *outfile = stdout;
    bool verbose = false;

    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': print_help(argv[0]); return 0;
        case 'v': verbose = true; break;
        case 'i':
            infile = fopen(optarg, "r");
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

    BitMatrix *Ht = create_transpose();

    // Scan bytes, encode and write lower and upper nibbles
    uint8_t dlo = 0; // last correctly decoded lower
    uint8_t dup = 0; // last correctly decoded upper
    int scan = 0;
    while ((scan = fgetc(infile)) != EOF) {
        // Bytes will come in pairs (lower,upper), first fget should catch EOF
        uint8_t lower = scan;
        uint8_t upper = fgetc(infile);

        HAM_STATUS slo = ham_decode(Ht, lower, &dlo);
        HAM_STATUS sup = ham_decode(Ht, upper, &dup);

        if (slo == HAM_CORRECT) {
            err_corrected++;
        }
        if (sup == HAM_CORRECT) {
            err_corrected++;
        }

        if (slo == HAM_ERR) {
            err_uncorrected++;
        }
        if (sup == HAM_ERR) {
            err_uncorrected++;
        }

        fputc(pack_byte(dup, dlo), outfile);
        bytes_processed += 2;
    }
    if (verbose) {
        fprintf(stderr, "Total bytes processed: %d\n", bytes_processed);
        fprintf(stderr, "Uncorrected errors: %d\n", err_uncorrected);
        fprintf(stderr, "Corrected errors: %d\n", err_corrected);
        double error_rate = err_uncorrected / (double) bytes_processed;
        fprintf(stderr, "Error rate: %-7f\n", error_rate);
    }

    // Free all dynamically allocated memory
    bm_delete(&Ht);
    fclose(infile);
    fclose(outfile);
    return 0;
}

// Create BitMatrix and sets bits for Hamming(8,4) parity check transpose
BitMatrix *create_transpose() {
    BitMatrix *Ht = bm_create(8, 4);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            if (i < 4 && i != j) {
                bm_set_bit(Ht, i, j);
            } else if (i >= 4 && (i - 4) == j) {
                bm_set_bit(Ht, i, j);
            }
        }
    }
    return Ht;
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
    printf("  A decoder for systematic Hamming(8, 4).\n\n");
    printf("USAGE\n");
    printf("  %s [-h] [-i infile] [-o outfile]\n\n", path);
    printf("OPTIONS\n");
    printf("  -%-14s Program usage and help.\n", "h");
    printf("  -%-14s Print statistics of decoding to stderr.\n", "v");
    printf("  -%-14s Specify input file to encode.\n", "i input");
    printf("  -%-14s Specify output file for encoded data.\n", "o output");
}
