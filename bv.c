#include "bv.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

BitVector *bv_create(uint32_t length) {
    BitVector *bv = malloc(sizeof(BitVector));
    if (bv) {
        bv->length = length;
        // account for remainder
        uint32_t nitems = (length % 8) ? length / 8 + 1 : length / 8;
        bv->vector = calloc(nitems, sizeof(uint8_t));
        if (!bv->vector) { 
            free(bv);
            bv = NULL;
        }
    }
    return bv;
}

void bv_delete(BitVector **v) {
    if (*v && (*v)->vector) {
        free((*v)->vector);
        free(*v);
        *v = NULL;
    }
}

uint32_t bv_length(BitVector *v) {
    return v->length;
}

void bv_set_bit(BitVector *v, uint32_t i) {
    assert(i < v->length);
    uint8_t byte = v->vector[i / 8];
    uint8_t mask = 0x1 << (i % 8);
    v->vector[i / 8] = byte | mask;
}

void bv_clr_bit(BitVector *v, uint32_t i) {
    assert(i < v->length);
    uint8_t byte = v->vector[i / 8];
    uint8_t mask = ~(0x1 << (i % 8));
    v->vector[i / 8] = byte & mask;
}

void bv_xor_bit(BitVector *v, uint32_t i, uint8_t bit) {
    assert(i < v->length);
    uint8_t byte = v->vector[i / 8];
    uint8_t mask = bit << (i % 8);
    v->vector[i / 8] = byte ^ mask;
}

uint8_t bv_get_bit(BitVector *v, uint32_t i) {
    assert(i < v->length);
    uint8_t byte = v->vector[i / 8];
    uint8_t val = byte >> (i % 8);
    return val & 0x1;
}

void bv_print(BitVector *v) {
    uint32_t num_bytes = (v->length % 8) ? v->length / 8 + 1 : v->length / 8;
    printf("Length: %d, Bytes: %d \n", v->length, num_bytes);
    for (uint32_t i = 0; i < num_bytes; i++) {
        printf("%x ", v->vector[i]);
    }
    printf("\n");
}
