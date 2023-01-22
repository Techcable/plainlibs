#include <limits.h>

#include "plain/minmax.h"

#include <criterion/criterion.h>
#include <criterion/new/assert.h>


Test(minmax, int_minmax) {
    cr_assert(eq(int, plain_min(1, 2), 1));
    cr_assert(eq(int, plain_max(1, 2), 2));
    cr_assert(eq(int, plain_max(72, INT_MAX), INT_MAX));
    cr_assert(eq(int, plain_min(-1, 7), -1));
}
