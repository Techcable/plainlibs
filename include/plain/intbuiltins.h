/**
 * Wrapper around integer builtins like nlz, add_overflow that are not required by the C standard.
 *
 * Uses intrinsics where possible Emulates integer builtins when missing.
 *
 * Dual-licensed under Creative Commons CC0 (Public Domain) and the MIT License.
 *
 * Do note a large portion of the fallback code here is based on
 * advice in Hacker's Delight (so big thank you to Henry S. Warren, Jr).
 *
 * Honestly I'm not very sure this code is copyrightable by me since so much 
 * of it is based on the book. I disclaim/open-source whatever small
 * contributions I have made here.
 * The underlying algorithms are not patented so I think this is probably good...
 *
 * For documentation, I have added references to GCC builtins, Rust integer functions,
 * and Java integer functions wherever appropriate :)
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
#ifndef PLAINLIBS_INTBUILTIN_H
#define PLAINLIBS_INTBUILTIN_H

#include <stdint.h>
#include <stdbool.h>
// Used for abs(int)
#include <stdlib.h>
#include <assert.h>

/*
 * I tend to make more reference to Java source than Rust
 *
 * This is because of `jshell`, not because I'm copy/pasting ;)
 */


/**
 * Fallback implementation of nlz(int32_t)
 *
 * This is used when compielr intrinsics are unavailable.
 */
static inline int _plainlib_int32_nlz_fallback(uint32_t x) {
    /*
     * See Hacker's Delight
     *
     * This is the simple algorithm. There is a branchless version.
     */
    uint32_t y;
    int n;
    n = 32;
    y = x >> 16; if (y != 0) {n = n -16; x = y;}
    y = x >>  8; if (y != 0) {n = n - 8; x = y;}
    y = x >>  4; if (y != 0) {n = n - 4; x = y;}
    y = x >>  2; if (y != 0) {n = n - 2; x = y;}
    y = x >>  1; if (y != 0) return n - 2;
    return n - x;
}

/**
 * Count the number of leading zeros in the specified integer.
 *
 * See also:
 * - GCC intrinsic __builtin_clz
 * - Java Integer.numberOfLeadingZeros
 * - Rust u32::leading_zeroes
 *
 * Undefined behavior if the specified value is zero (matching GCC behavior).
 */
static inline int plainlib_int32_nlz(uint32_t val) {
    assert(val != 0);
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_clz(val);
    #else
        return _plainlib_int32_nlz_fallback(val);
    #endif
}

/**
 * Fallback implementation of nlz(int64_t)
 *
 * This is used when compiler intrinsics are unavailable.
 */
static inline int _plainlib_int64_nlz_fallback(uint64_t x) {
    /*
     * See Hacker's Delight
     *
     * This is the simple algorithm, ported to 64 bits.
     */
    uint64_t y;
    int n = 64;
    y = x >> 32; if (y != 0) {n = n - 32; x = y;}
    y = x >> 16; if (y != 0) {n = n - 16; x = y;}
    y = x >>  8; if (y != 0) {n = n - 8; x = y;}
    y = x >>  4; if (y != 0) {n = n - 4; x = y;}
    y = x >>  2; if (y != 0) {n = n - 2; x = y;}
    y = x >>  1; if (y != 0) return n - 2;
    return n - ((int) x);
}

/*
 * Count the number of leading zeros in the specified integer.
 * 
 * See also:
 * - GCC intrinsic __builtin_clzll
 * - Java Long.numberOfLeadingZeros
 * - Rust u64::leading_zeros
 *
 *
 * Undefined behavior if the specified value is zero (matching GCC behavior).
 */
static inline int plainlib_int64_nlz(uint64_t val) {
    assert(val != 0);
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_clzll((unsigned long long) val);
    #else
        return _plainlib_int64_nlz_fallback(val);
    #endif
}


/*
 * Overflow checking arithmetic operations
 *
 * Emulates GCC builtins: https://gcc.gnu.org/onlinedocs/gcc/Integer-Overflow-Builtins.html
 *
 * See also Hackers Delight Chapter 2.13 "Overflow Checking".
 *
 * NOTE: In the C standard, signed integer overflow is UB
 * but unsigned is twos complement.
 */

/**
 * Fallback implementation of addExact(int, int)
 */
static inline bool _plainlib_int32_adds_fallback(int32_t first, int32_t second, int32_t *res) {
    /*
     * Assume 64 bit machine, promote to 64 bit integers
     *
     * This will never overflow and uses 2-4 cycles (depending on whether you count sign extension)
     */
    int64_t bigres = ((int64_t) first) + ((int64_t) second);
    int32_t castres = (int32_t) bigres;
    *res = castres;
    return ((int64_t) castres) != bigres;

}

/**
 * Signed integer addition, checking for overflow.
 * 
 * Returns true if overflow occurs.
 * 
 * The result is computed using twos complement wrapping,
 * and it is computed unconditionally.
 * 
 * See also:
 * - GCC builtin __builtin_add_overflow()
 * - Java Math.addExact(int, int)
 * - Rust i32::overflowing_add
 */
static inline bool plainlib_int32_adds(int32_t first, int32_t second, int32_t *res) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_add_overflow(first, second, res);
    #else
        return _plainlib_int32_adds_fallback(first, second, res);
    #endif
}

/**
 * Fallback implmentation of subtractExact(int, int)
 */
static inline bool _plainlib_int32_subs_fallback(int32_t first, int32_t second, int32_t *res) {   
    /*
     * Assume 64 bit machine, promote to 64 bit integers
     *
     * This will never overflow and uses 2-4 cycles (depending on whether you count sign extension)
     */
    int64_t bigres = ((int64_t) first) - ((int64_t) second);
    int32_t castres = (int32_t) bigres;
    *res = castres;
    return ((int64_t) castres) != bigres;
}


/**
 * Signed integer subtraction, checking for overflow.
 * 
 * Returns true if overflow occurs.
 * 
 * The result is computed using twos complement wrapping,
 * and it is computed unconditionally.
 * 
 * See also:
 * - GCC builtin __builtin_sub_overflow()
 * - Java Math.subExact(int, int)
 * - Rust i32::overflowing_add
 */
static inline bool plainlib_int32_subs(int32_t first, int32_t second, int32_t *res) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_sub_overflow(first, second, res);
    #else
        return _plainlib_int32_subs_fallback(first, second, res);
    #endif
}

/**
 * Fallback implemnetaiton of multiplyExact(int, int)
 */
static inline bool _plainlib_int32_muls_fallback(int32_t first, int32_t second, int32_t *res) {
    /*
     * Assume 64 bit machine, promote to 64 bit integers
     * 
     * The fact this will never overflow is harder to prove than with add/sub,
     * but it is indeed true.
     *
     * Ignoring sign, 32 bit integers can only have magnitude (absolute value)
     * of 2^31 at most (ie. Math.abs(Integer.MAX_VALUE)).
     * By exponent rules, x^a * x^b = x^(a+b)
     * By similar rules, multipling 2^31 by 2^31 (our worst case) only produce 2^62 at most.
     * for our magintude.
     * This easily fits into a 64 bit (signed) integer.
     *
     * Emperically, I have tried Java Math.multiplyExact(long, long) with every combination
     * of Ingeger.MAX_VALUE and Integer.MIN_VALUE, but it never triggers an exception ;)
     *
     * On 32 bit machines, this assumes the compiler has inteligent implementation
     * and uses multiply/high low. This is most likely true and optimizes away.
     * However I have not checked output ;)
     */
    int64_t bigres = ((int64_t) first) * ((int64_t) second);
    int32_t castres = (int32_t) bigres;
    *res = castres;
    return ((int64_t) castres) != bigres;
}

/**
 * Signed integer multiplication, checking for overflow.
 * 
 * Returns true if overflow occurs.
 * 
 * The result is computed using twos complement wrapping,
 * and it is computed unconditionally.
 * 
 * See also:
 * - GCC builtin __builtin_mul_overflow()
 * - Java Math.mulExact(int, int)
 * - Rust i32::overflowing_mul
 */
static inline bool plainlib_int32_muls(int32_t first, int32_t second, int32_t *res) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_mul_overflow(first, second, res);
    #else
        return _plainlib_int32_muls_fallback(first, second, res);
    #endif
}

/**
 * Fallback implementation of addExact(long, long)
 */
static inline bool _plainlib_int64_adds_fallback(int64_t first, int64_t second, int64_t *res) {
    /*
     * See Hacker's Delight Chapter 2-13 section "Signed Add/Subtract".
     * 
     * NOTE: In the C standard, signed integer overflow is UB
     */ 
    uint64_t ufirst = (uint64_t) first;
    uint64_t usecond = (uint64_t) second;
    uint64_t ures = ufirst + usecond;
    *res = (int32_t) ures;
    return (((ures ^ ufirst) & (ures ^ usecond)) >> 63) != 0;
}

/**
 * Signed integer addition, checking for overflow.
 * 
 * Returns true if overflow occurs.
 *
 * The result is computed using twos complement wrapping,
 * and it is computed unconditionally.
 * 
 * See also:
 * - GCC builtin __builtin_add_overflow()
 * - Java Math.addExact(long, long)
 * - Rust i64::overflowing_add
 */
static inline bool plainlib_int64_adds(int64_t first, int64_t second, int64_t *res) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_add_overflow(first, second, res);
    #else
        return _plainlib_int64_adds_fallback(first, second, res);
    #endif
}


/**
 * Fallback implementation of subtractExact(long, long)
 */
static inline bool _plainlib_int64_subs_fallback(int64_t first, int64_t second, int64_t *res) {    
    /*
     * See Hacker's Delight Chapter 2-13 section "Signed Add/Subtract".
     * 
     * NOTE: In the C standard, signed integer overflow is UB
     */ 
    uint64_t ufirst = (uint64_t) first;
    uint64_t usecond = (uint64_t) second;
    uint64_t ures = ufirst - usecond;
    *res = (int64_t) ures;
    return (((ufirst ^ usecond) & (ures ^ ufirst)) >> 63) != 0;
}


/**
 * Signed integer subtraction, checking for overflow.
 * 
 * Returns true if overflow occurs.
 *
 * The result is computed using twos complement wrapping,
 * and it is computed unconditionally.
 * 
 * See also:
 * - GCC builtin __builtin_sub_overflow()
 * - Java Math.subtractExact(long, long)
 * - Rust i64::overflowing_sub
 */
static inline bool plainlib_int64_subs(int64_t first, int64_t second, int64_t *res) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_sub_overflow(first, second, res);
    #else
        return _plainlib_int64_subs_fallback(first, second, res);
    #endif
}


/**
 * Fallback implementation of subtractExact(long, long)
 * 
 * This is complicated....
 */
static inline bool _plainlib_int64_muls_fallback(int64_t first, int64_t second, int64_t *res) {
    /*
     * See Hacker's Delight Chapter 2-13 section "Multiplication".
     *
     * First we would want to check the result would "definitely overflow"
     * or "definitely not overlfow".
     * This requires comparing the number of leading zeros.
     *
     * For signed integer multiplcation, This calls the nlz functions 4 times.
     * 
     * Considering that this is the fallback case for no compiler support
     * for checking multiplication overflow, there is probably no support
     * for a builtin nlz instruction either :(
     *
     * Fallback nlz instruction looks like 7-8 cycles with possible branching,
     * which is not much better than integer division.
     *
     * Therefore, we do a more sloppy test based on taking the absolute values (see below).
     *
     * NOTE: Regardless of whether or not the result overflows,
     * we are still required to compute it.
     *
     * We do this by casting to unsigned integers because:
     * 1. In C, signed multiplication overflow is undefined behavior but unsigned is not
     * 2. In twos complement, signed and unsigned multiplication
     */
    uint64_t ufirst = (uint64_t) first;
    uint64_t usecond = (uint64_t) second;
    uint64_t ures = ufirst * usecond;
    *res = (int64_t) ures;
    /*
     * Guard for Integer.MIN_VALUE because absolute value of that is undefined 
     * according to the C standard :(
     *
     * In that case, the result overflows if other > 1, but is fine otherwise.
     */
    if (first == INT64_MIN) return usecond > 1;
    if (second == INT64_MIN) return ufirst > 1;
    uint64_t first_abs = (uint64_t) llabs((long long) first);
    uint64_t second_abs = (uint64_t) llabs((long long) second);
    assert(first_abs >= 0 && second_abs >= 0);
    /*
     * Overflow is impossible if
     * A. *either* absolute value <= 1,
     * B. if *both* absolute values fit in 31 bits.
     *
     * Part A is trivial.
     * Part B uses a similar justification as why 32 bit integer multiplication can
     * never overflow an 64 bit integer
     */
     bool overflow_impossible = ((first_abs <= 1) | (second_abs <= 1)) 
        || (((first_abs >> 31) == 0) & ((second_abs >> 31) == 0)); 
    if (overflow_impossible) {
        return false;
    } else {
        assert(second != 0); // Checked for in 'overflow_impossible'
        /*
         * Do a more precise test based on integer division.
         *
         * See Hacker's Delight Chapter 2-13, section "Integer Multiplication".
         *
         * We have already checked for the easy case where.
         *
         * I believe on x86_64, division is about 9 cycles, so this is still
         * better than using the fallback implementation of nlz.
         *
         * Of course everything changes if nlz is a CPU builtin.
         * In that case we should use the other implmenetaiton....
         */
        int64_t c = ((~(first ^ second)) >> 31) + (1LL << 31);
        return first_abs > (c / second_abs);
    }
}


/**
 * Signed integer subtraction, checking for overflow.
 * 
 * Returns true if overflow occurs.
 *
 * The result is computed using twos complement wrapping,
 * and it is computed unconditionally.
 * 
 * See also:
 * - GCC builtin __builtin_mul_overflow()
 * - Java Math.multiplyExact(long, long)
 * - Rust i64::overflowing_mul
 */
static inline bool plainlib_int64_muls(int64_t first, int64_t second, int64_t *res) {
    #if defined(__GNUC__) || defined(__clang__)
        return __builtin_mul_overflow(first, second, res);
    #else
        return _plainlib_int64_muls(first, second, res);
    #endif
}

#endif /* PLAINLIBS_INTBUILTIN_H */
