#include "hamming.h"

#include "bm.h"

#include <stdint.h>

static HAM_STATUS status_table[] = { HAM_OK, 4, 5, HAM_ERR, 6, HAM_ERR, HAM_ERR, 3, 7, HAM_ERR,
    HAM_ERR, 2, HAM_ERR, 1, 0, HAM_ERR };

uint8_t ham_encode(BitMatrix *G, uint8_t msg) {
    BitMatrix *m = bm_from_data(msg, 4);
    BitMatrix *p = bm_multiply(m, G);
    uint8_t code = bm_to_data(p);
    bm_delete(&m);
    bm_delete(&p);
    return code;
}

HAM_STATUS ham_decode(BitMatrix *Ht, uint8_t code, uint8_t *msg) {
    BitMatrix *c = bm_from_data(code, 8);
    BitMatrix *p = bm_multiply(c, Ht);
    uint8_t synd = bm_to_data(p);
    bm_delete(&c);
    bm_delete(&p);
    HAM_STATUS status = status_table[synd];
    if (status >= 0) {
        uint8_t corrected = code ^ (0x1 << status); // flip identified bit
        *msg = corrected & 0xf; // grab lower nibble
        return HAM_CORRECT;
    } else if (status == HAM_OK) {
        *msg = code & 0xf;
    }
    return status;
}
