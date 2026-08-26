/**
 * @file bignum_shift_left.c
 * @brief C11 reference implementation for bignum_shift_left.
 * @details Validates inputs, computes into a stack-local temporary, normalizes
 * the result, and publishes it only after successful completion. The function
 * is deterministic, allocation-free, and safe for independent concurrent calls.
 */
#include "bignum_shift_left.h"
#include <string.h>

bignum_shift_status_t bignum_shift_left(bignum_t *restrict num, size_t shift_amount)
{
    bignum_t tmp = {0};
    size_t word_shift, bit_shift;
    if (num == NULL) return BIGNUM_SHIFT_ERROR_NULL_ARG;
    if (shift_amount == 0U) return BIGNUM_SHIFT_SUCCESS;
    if (num->len == 0U) return BIGNUM_SHIFT_SUCCESS;
    if (num->len > BIGNUM_CAPACITY || shift_amount >= (size_t)BIGNUM_CAPACITY * 64U)
        return BIGNUM_SHIFT_ERROR_OVERFLOW;
    word_shift = shift_amount / 64U; bit_shift = shift_amount % 64U;
    if (num->len == BIGNUM_CAPACITY &&
        ((word_shift != 0U && num->words[BIGNUM_CAPACITY - 1U] != 0U) ||
         (bit_shift != 0U && (num->words[BIGNUM_CAPACITY - 1U] >> (64U - bit_shift)) != 0U)))
        return BIGNUM_SHIFT_ERROR_OVERFLOW;
    for (size_t i = 0U; i < num->len; ++i) {
        size_t dst = i + word_shift;
        uint64_t value = num->words[i];
        if (bit_shift != 0U) {
            if (dst < BIGNUM_CAPACITY) tmp.words[dst] |= value << bit_shift;
            if (dst + 1U < BIGNUM_CAPACITY) tmp.words[dst + 1U] |= value >> (64U - bit_shift);
        } else if (dst < BIGNUM_CAPACITY) tmp.words[dst] = value;
    }
    tmp.len = BIGNUM_CAPACITY;
    while (tmp.len > 0U && tmp.words[tmp.len - 1U] == 0U) --tmp.len;
    if (tmp.len == 0U) tmp.len = 1U;
    *num = tmp;
    return BIGNUM_SHIFT_SUCCESS;
}
