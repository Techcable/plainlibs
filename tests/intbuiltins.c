#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "plain/intbuiltins.h"

union nlz_func {
    int (*nlz32)(uint32_t);
    int (*nlz64)(uint64_t);
};

static void test_nlz(union nlz_func func, int bits) {
    if (bits != 32 && bits != 64) cr_fatal("Invalid bits");
    const uint64_t max = bits == 32 ? UINT32_MAX : UINT64_MAX;
    #define nlz(x) (bits == 32 ? (*func.nlz32)((uint32_t) x) : (*func.nlz64)(x));
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
    int (*nlz)(uint32_t) = _plainlib_int32_nlz_fallback;
    cr_assert(eq(i32, nlz(0), 32));
    union nlz_func func = {.nlz32=nlz};
    test_nlz(func, 32);
}


Test(intbuiltins, nlz32) {
    int (*nlz)(uint32_t) = plainlib_int32_nlz;
    union nlz_func func = {.nlz32=nlz};
    test_nlz(func, 32);
}


Test(intbuiltins, nlz_fallback64) {
    // Fallback actually has defined behavior for nlz(0). GCC does not
    int (*nlz)(uint64_t) = _plainlib_int64_nlz_fallback;
    cr_assert(eq(i32, nlz(0), 64));
    union nlz_func func = {.nlz64=nlz};
    test_nlz(func, 64);
}

Test(intbuiltins, nlz64) {
    int (*nlz)(uint64_t) = plainlib_int64_nlz;
    union nlz_func func = {.nlz64=nlz};
    test_nlz(func, 64);
}
