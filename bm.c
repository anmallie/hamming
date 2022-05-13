#include "bm.h"

#include "bv.c"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct BitMatrix {
    uint32_t rows;
    uint32_t cols;
    BitVector *vector;
};

BitMatrix *bm_create(uint32_t rows, uint32_t cols) {
    BitMatrix *bm = malloc(sizeof(BitMatrix));
    if (bm) {
        bm->rows = rows;
        bm->cols = cols;
        bm->vector = bv_create(rows * cols);
        if (!bm->vector) {
            free(bm);
            bm = NULL;
        }
    }
    return bm;
}

void bm_delete(BitMatrix **m) {
    if ((*m) && (*m)->vector) {
        bv_delete(&(*m)->vector);
        free(*m);
        *m = NULL;
    }
}

uint32_t bm_rows(BitMatrix *m) {
    return m->rows;
}

uint32_t bm_cols(BitMatrix *m) {
    return m->cols;
}

void bm_set_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    assert(r < m->rows && c < m->cols);
    bv_set_bit(m->vector, r * (m->cols) + c);
}

void bm_clr_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    assert(r < m->rows && c < m->cols);
    bv_clr_bit(m->vector, r * (m->cols) + c);
}

uint8_t bm_get_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    assert(r < m->rows && c < m->cols);
    return bv_get_bit(m->vector, r * (m->cols) + c);
}

BitMatrix *bm_from_data(uint8_t byte, uint32_t length) {
    if (length > 8) {
        return (BitMatrix *) 0;
    }
    BitMatrix *bm = bm_create(1, length);
    if (!bm) {
        return bm;
    }
    for (uint32_t i = 0; i < length; i++) {
        if (0x1 & (byte >> i)) {
            bm_set_bit(bm, 0, i);
        }
    }
    return bm;
}

uint8_t bm_to_data(BitMatrix *m) {
    assert(m->rows == 1 && m->cols <= 8);
    uint8_t data = 0;
    for (uint32_t i = 0; i < m->cols; i++) {
        data |= bm_get_bit(m, 0, i) << i;
    }
    return data;
}

BitMatrix *bm_multiply(BitMatrix *A, BitMatrix *B) {
    assert(A->cols == B->rows);
    BitMatrix *result = bm_create(bm_rows(A), bm_cols(B));
    for (uint32_t i = 0; i < bm_rows(result); i++) {
        for (uint32_t j = 0; j < bm_cols(result); j++) {
            uint8_t entry = 0;
            for (uint32_t k = 0; k < bm_cols(A); k++) {
                entry ^= bm_get_bit(A, i, k) & bm_get_bit(B, k, j);
            }
            if (entry) {
                bm_set_bit(result, i, j);
            }
        }
    }
    return result;
}

void bm_print(BitMatrix *m) {
    printf("Rows: %d, Cols: %d\n", m->rows, m->cols);
    for (uint32_t i = 0; i < m->rows; i++) {
        for (uint32_t j = 0; j < m->cols; j++) {
            printf("%s", bm_get_bit(m, i, j) ? "1" : "0");
        }
        printf("\n");
    }
}
