#include "plain/intmath.h"

#include <criterion/criterion.h>
#include <criterion/new/assert.h>


static void assert_pow64s_overflowing(
    int64_t a, uint32_t b,
    int64_t expected_res,
    bool expected_overflow
) {
    int64_t actual_res = 0;
    bool actual_overflow = plain_int_pow64s_overflowing(a, b, &actual_res);
    cr_assert(eq(int, actual_overflow != 0, expected_overflow != 0),
              "Epected overflow = %s for %lld * %lld = %lld, but got %s",
              expected_overflow ? "true" : "false",
              a,
              b,
              expected_res,
              actual_overflow ? "true" : "false");
    cr_assert(
        eq(i64, actual_res, expected_res),
        "Expected res = %lld for %lld**%d, but got %lld",
        expected_res, a, b, actual_res
    );
}

Test(intmath, int_pow64s) {
    // quick smoke tests
    assert_pow64s_overflowing(17, 6, 24137569, false);
    assert_pow64s_overflowing(17, 30, 5402864989787419873, true);
    assert_pow64s_overflowing(8, 9, 134217728, false);
    assert_pow64s_overflowing(5, 60, 8512967443501092241, true);
    assert_pow64s_overflowing(5, 27, 7450580596923828125, false);
}
