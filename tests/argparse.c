#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include "plain/argparse.h"

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-value"
#endif

struct simple_flags {
    char *foo;
    bool bar;
    bool baz;
    char* long_only_value;
};


static void parse_simple_flags(struct arg_parser *parser, struct simple_flags *flags) {
    while (has_flag_args(parser)) {
        static const char* FOO_ALIASES[] = {"foozie", NULL};
        static const char* BAZ_ALIASES[] = {"bazzie", NULL};
        static const struct arg_config FOO_CONFIG = {.short_name = "f", .aliases = FOO_ALIASES};
        static const struct arg_config BAR_CONFIG = {.flag = true, .short_name = "b"};
        // NOTE: Has no short name, only aliases (bazzy)
        static const struct arg_config BAZ_CONFIG = {.flag = true, .aliases = BAZ_ALIASES};
        if (match_arg(parser, "foo", &FOO_CONFIG)) {
            cr_assert(ne(ptr, parser->current_value, NULL));
            flags->foo = parser->current_value;
            fprintf(stderr, "Found foo\n");
        } else if (match_arg(parser, "bar", &BAR_CONFIG)) {
            cr_assert(eq(ptr, parser->current_value, NULL));
            flags->bar = true;
            fprintf(stderr, "Found bar\n");
        } else if (match_arg(parser, "baz", &BAZ_CONFIG)) {
            cr_assert(eq(ptr, parser->current_value, NULL));
            fprintf(stderr, "Found baz\n");
            flags->baz = true;
        } else if (match_arg(parser, "long-only-value", NULL)) {
            cr_assert(ne(parser->current_value, NULL));
            flags->long_only_value = parser->current_value;
        } else {
            cr_fail("Unknown flag: %s", current_arg(parser));
        }
    }
}
static void assert_flags_equal(struct simple_flags *expected, struct simple_flags *actual) {
    cr_assert(eq(str, expected->foo, actual->foo));
    cr_assert(eq(u8, expected->bar, actual->bar), "Expected --bar=%d, but got --bar=%d", expected->bar, actual->bar);
    cr_assert(eq(u8, expected->baz, actual->baz), "Expected --baz=%d, but got --baz=%d", expected->baz, actual->baz);
    cr_assert(eq(str, expected->long_only_value, actual->long_only_value));
}
#define POS_BUF_SIZE 12
static size_t parse_positional(struct arg_parser *parser, char *buf[POS_BUF_SIZE]) {
    size_t len = 0;
    cr_assert(not(has_flag_args(parser)), "Has flag args (but expected positional)");
    while (has_args(parser)) {
        cr_assert(lt(uptr, len, POS_BUF_SIZE), "Expected <= %lld args, but got more", POS_BUF_SIZE);
        char *value = consume_arg(parser);
        cr_assert(ne(ptr, value, NULL));
        buf[len++] = value;
    }
    return len;
}

Test(argparse, empty_args) {
    static char *ARGS[] = {"exe", NULL};
    struct arg_parser parser = init_args(1, ARGS);
    cr_assert(not(has_flag_args(&parser)));
    cr_assert(not(has_args(&parser)));
}

Test(argparse, only_positional) {
    static char *ARGS[] = {"exe", "going", "poop", "-17", "--", NULL};
    struct arg_parser parser = init_args(5, ARGS);
    cr_assert(not(has_flag_args(&parser)));
    char *buf[POS_BUF_SIZE];
    size_t actual = parse_positional(&parser, buf);
    cr_assert(eq(uptr, actual, 4));
    for (int i = 0; i < 4; i++) {
        cr_assert(eq(str, ARGS[i + 1], buf[i]));
    }
}

Test(argparse, basic_flags) {
    static char *ARGS[] = {"exe", "--foo", "foot", "--baz", "-b" /* bar */, NULL};
    struct simple_flags expected_flags = {
        .foo = "foot",
        .bar = true,
        .baz = true,
    };
    struct arg_parser parser = init_args(5, ARGS);
    struct simple_flags actual_flags = {0};
    parse_simple_flags(&parser, &actual_flags);
    cr_assert(not(has_args(&parser)));
    assert_flags_equal(&expected_flags, &actual_flags);
}

#ifdef __clang__
  #pragma clang diagnostic pop
#endif
