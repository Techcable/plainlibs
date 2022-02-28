/**
 * Miscelaneous math functions releated to integers.
 * 
 * Requires "intbuiltins.h"
 *
 * Dual-licensed under Creative Commons CC0 (Public Domain) and the MIT License.
 *
 * Do note a large portion of the code here is based on
 * advice in Hacker's Delight (so big thank you to Henry S. Warren, Jr).
 *
 * Honestly I'm not very sure this code is copyrightable by me since so much
 * of it is based on the book. I disclaim/open-source whatever small
 * contributions I have made here.
 * The underlying algorithms are not patented so I think this is probably good...
 *
 * Source code & issue tracker: https://github.com/Techcable/plainlibs
 *
 * VERSION: 0.1.0-beta.3-dev
 *
 * CHANGELOG:
 *
 * NEXT:
 * - Initial release
 */
#ifndef PLAINLIBS_INTMATH_H
#define PLAINLIBS_INTMATH_H

#include "plain/intbuiltins.h"


/*
 * Okay. Exponentation by squaring is a pretty simple idea.
 *
 * I'm really proud of this (long?) explination so bear with me
 * or email me if you're confused or something :laugh:
 *
 * Exponentation by squaring takes advantage of the binary representation of `exp`
 * and the fact that x^a * x^b = x^(a+c) (where a^b means exponentation not xor).
 *
 * Assume exp is 7 = 0b0111 = 4+2+1=(2^2)+(2^1)+(2^0)
 *
 * We can split up x^7 = x^4 * x^2 * x^1 = x^(4+2+1)
 *
 * Notice also how future results (x^4) can be expressed as squares or fourths of previous ones.
 *
 * The following python implementation works correctly:
 * def powi(base, exp):
 *     bits = bin(exp)[2:]
 *     res = 1
 *     for idx, bit in enumerate(bits):
 *          idx_power = 1 << idx # 2**idx
 *          if bit == '1':
 *              res *= (x**idx_power)
 *     return res
 *
 * Byeond the obvious slowness/overhead of the Python,
 * there is one major problem with the approach: It begs the question.
 *
 * More specifically, we will end up calling x**4 or x**2 in the middle
 * of the loop, so we encounter a circular definition or inifinite recursion.
 *
 * However, because each idx is + 1 the last, each idx_power is double the last one.
 * This means each x**idx_power == (x**(idx_power/2))**2
 *
 * We unrool a single loop iteration and start at idx=1
 *
 * Rewriting:
 * def powi(base, exp):
 *     if exp == 0: return 1 # Guard for 2**x
 *     bits = bin(exp)[2:]
 *     res = 1
 *     # unrolled iteration idx=0, bit=bits[0]
 *     # idx_power = 1 << 0 = 1 # 2**0
 *     current_power = base # base**(idx_power) == x**1 = x
 *     if bits[0] == '1':
 *           res *= base
 *     # further iterations after unrolled
 *     for bit in (bits[1:]):
 *         # This is the key to avoiding the (x**idx_power) computation,
 *         current_power *= current_power
 *         if bit == '1':
 *              res *= current_power
 *     return res
 *
 *
 * We want to avoid creating the iterator object (and temporary bit storage).
 *
 * Switch from for each iterating in the form `for bit in bits`
 * to explicit use of iterator `while (!remainingBits.isEmpty())`
 *
 * Or iterating over `remainingBitsBinary`
 *
 * !remainingBits.isEmpty() == remainingBitsBinary > 0
 * 
 * Usage of bits[:1] can be expressed as len(remainingBits) >= 2
 * or remainingBitsBinary >= 2
 *
 * remainingBits.pop() is remainingBits >>= 1
 *
 * This gives us
 * 
 * def powi(base, exp):
 *     if exp == 0: return 1 # Guard for 2**x
 *     remainingBits = exp
 *     res = 1
 *     # unrolled iteration idx=0, bit=bits[0]
 *     # idx_power = 1 << 0 = 1 # 2**0
 *     current_power = base # base**(idx_power) == x**1 = x
 *     if (remainingBits & 1) == 1: # bits[0] == '1'
 *           res *= base
 *     # further iterations after unrolled
 *     while remainingBits >= 2: # len(remainingBits) >= 2
 *         # This is the key to avoiding the (x**idx_power) computation,
 *         current_power *= current_power
 *         if (remainingBits & 2) != 0:
 *              res *= current_power
 *         remainingBits >>= 1
 *     return res
 *
 * See also Hacker's Delight 11-3. The section is very brief.
 */
#define _IMPL_EXP_BY_SQUARING_OVERFLOWING(tp, mul_op) do { \
        if (exp == 0) { \
            *res = 1; \
            return false; \
        } \
        bool overflowing = false; \
        uint32_t remaining_bits = exp; \
        tp current_res = 1; \
        tp current_power = base; \
        if (remaining_bits & 1) { \
            current_res = base; \
        } \
        while (remaining_bits >= 2) { \
            overflowing |= mul_op(current_power, current_power, &current_power); \
            if (remaining_bits & 2) { \
                overflowing |= mul_op(current_res, current_power, &current_res); \
            } \
            remaining_bits >>= 1; \
        } \
        *res = current_res; \
        return overflowing; \
    } while (false)

/**
 * Raises `base` to the power of `exp`, using exponentiation by squaring.
 * 
 * Returns true if overflow occurred, false if it has not.
 *
 * This mirrors Rust's i64::overflowing_pow
 */
static inline bool plain_int_pow64s_overflowing(int64_t base, uint32_t exp, int64_t *res) {
    _IMPL_EXP_BY_SQUARING_OVERFLOWING(int64_t, plain_int_overflowing_mul64s);
}


/**
 * Raises `base` to the power of `exp`, using exponentiation by squaring.
 * 
 * Wraps around on the boundary of the type, ignoring overflow.
 * 
 * This mirrors Rust's i64::wrapping_pow
 */
static inline int64_t plain_int_pow64s_wrapping(int64_t base, uint32_t power) {
    int64_t res;
    // This relies on DCE being good ;)
    plain_int_pow64s_overflowing(base, power, &res);
    return res;
}

#endif /* PLAINLIBS_INTMATH_H */
