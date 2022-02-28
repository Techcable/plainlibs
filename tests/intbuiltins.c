#include "plain/intbuiltins.h"

#include <criterion/criterion.h>
#include <criterion/new/assert.h>

union nlz_func {
    int (*nlz32)(uint32_t);
    int (*nlz64)(uint64_t);
};

static void test_nlz(union nlz_func func, int bits) {
    if (bits != 32 && bits != 64)
        cr_fatal("Invalid bits");
    const uint64_t max = bits == 32 ? UINT32_MAX : UINT64_MAX;
#define nlz(x) (bits == 32 ? (*func.nlz32)((uint32_t)x) : (*func.nlz64)(x));
    cr_assert(eq(i32, nlz(max), 0));
    cr_assert(eq(i32, nlz(max - 1), 0));
    cr_assert(eq(i32, nlz(max - 1), 0));
    cr_assert(eq(i32, nlz(max / 2), 1));
    cr_assert(eq(i32, nlz(1 << 15), bits - 15 - 1));
    cr_assert(eq(i32, nlz(1), bits - 1));
#undef nlz
}

Test(intbuiltins, nlz_fallback32) {
    // Fallback actually has defined behavior for nlz(0). GCC does not
    int (*nlz)(uint32_t) = _plainlib_int_nlz32_fallback;
    cr_assert(eq(i32, nlz(0), 32));
    union nlz_func func = {.nlz32 = nlz};
    test_nlz(func, 32);
}

Test(intbuiltins, nlz32) {
    int (*nlz)(uint32_t) = plainlib_int_nlz32;
    union nlz_func func = {.nlz32 = nlz};
    test_nlz(func, 32);
}

Test(intbuiltins, nlz_fallback64) {
    // Fallback actually has defined behavior for nlz(0). GCC does not
    int (*nlz)(uint64_t) = _plainlib_int_nlz64_fallback;
    cr_assert(eq(i32, nlz(0), 64));
    union nlz_func func = {.nlz64 = nlz};
    test_nlz(func, 64);
}

Test(intbuiltins, nlz64) {
    int (*nlz)(uint64_t) = plainlib_int_nlz64;
    union nlz_func func = {.nlz64 = nlz};
    test_nlz(func, 64);
}

typedef bool (*Int64CheckedOp)(int64_t, int64_t, int64_t*);

static void assert_mul_overflowing(Int64CheckedOp target, int64_t a, int64_t b, bool expected_overflow) {
    /*
     * Taking advantage of twos complement wrapping,
     * finding the actual result is easy (regardless of whether or not overflow).
     *
     * Mind the fact that signed overflow is UB.
     */
    int64_t expected_res = (int64_t)(((uint64_t)a) * ((uint64_t)b));
    int64_t actual_res = 0;
    bool actual_overflow = target(a, b, &actual_res);
    cr_assert(eq(int, actual_overflow != 0, expected_overflow != 0),
              "Epected overflow = %s for %lld * %lld = %lld, but got %s",
              expected_overflow ? "true" : "false",
              a,
              b,
              expected_res,
              actual_overflow ? "true" : "false");
    cr_assert(eq(i64, actual_res, expected_res));
}

/*
 * Too lazy to test all the other overflow ops.
 *
 * Focus on 64 bits and especially multiplication.
 */

Test(intbuiltins, muls_i64_fallback) {
    Int64CheckedOp target = _plainlib_int_checked_mul64s_fallback;
    // quick smoke tests
    assert_mul_overflowing(target, INT64_MAX, 1, false);
    assert_mul_overflowing(target, INT64_MAX / 2, 2, false);
    assert_mul_overflowing(target, INT64_MAX, 0, false);
    assert_mul_overflowing(target, INT64_MAX, 2, true);
    assert_mul_overflowing(target, INT64_MAX - 1, 2, true);
    assert_mul_overflowing(target, INT64_MAX - 1, 2, true);
    assert_mul_overflowing(target, INT64_MIN, -1, true);
    assert_mul_overflowing(target, INT64_MIN, -2, true);
    assert_mul_overflowing(target, INT64_MIN / 2, -2, true);
    assert_mul_overflowing(target, INT64_MIN / 2, 2, false);
}
