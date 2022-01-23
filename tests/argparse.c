#include <assert.h>

#include "plain/argparse.h"

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
            assert(parser->current_value != NULL);
            flags->foo = parser->current_value;
        } else if (match_arg(parser, "bar", &BAR_CONFIG)) {
            assert(parser->current_value == NULL);
            flags->bar = true;
        } else if (match_arg(parser, "baz", &BAZ_CONFIG)) {
            assert(parser->current_value == NULL);
            flags->baz = true;
        } else if (match_arg(parser, "long-only-value", NULL)) {
            assert(parser->current_value != NULL);
            flags->long_only_value = parser->current_value;
        } else {
            fprintf(stderr, "Unknown flag: %s\n", current_arg(parser));
            abort();
        }
    }
}

int nullsafe_strcmp(char *first, char *second) {
    if (first == NULL) {
        if (second == NULL) return 0;
        else return 1;
    } else if (second == NULL) return 1;
    else {
        return strcmp(first, second);
    }
}


static void assert_flags_equal(struct simple_flags *expected, struct simple_flags *actual) {
    assert(nullsafe_strcmp(expected->foo, actual->foo) == 0);
    assert(expected->bar == actual->bar);
    assert(expected->baz == actual->baz);
    assert(nullsafe_strcmp(expected->long_only_value, actual->long_only_value) == 0);
}
#define POS_BUF_SIZE 12
static size_t parse_positional(struct arg_parser *parser, char *buf[POS_BUF_SIZE]) {
    size_t len = 0;
    assert(!has_flag_args(parser)); // Has flag args (but expected positional)
    while (has_args(parser)) {
        assert(len < POS_BUF_SIZE); // Expected < POS_BUF_SIZE args
        char *value = consume_arg(parser);
        assert(value != NULL);
        buf[len++] = value;
    }
    return len;
}

void test_empty_args() {
    static char *ARGS[] = {"exe", NULL};
    struct arg_parser parser = init_args(1, ARGS);
    assert(!has_flag_args(&parser));
    assert(!has_args(&parser));
}

void test_only_positional() {
    static char *ARGS[] = {"exe", "going", "poop", "-17", "--", NULL};
    struct arg_parser parser = init_args(5, ARGS);
    assert(!has_flag_args(&parser));
    char *buf[POS_BUF_SIZE];
    size_t actual = parse_positional(&parser, buf);
    assert(actual == 4);
    for (int i = 0; i < 4; i++) {
        assert(strcmp(ARGS[i + 1], buf[i]) == 0);
    }
}

void test_basic_flags() {
    static char *ARGS[] = {"exe", "--foo", "foot", "--baz", "-b" /* bar */, NULL};
    struct simple_flags expected_flags = {
        .foo = "foot",
        .bar = true,
        .baz = true,
    };
    struct arg_parser parser = init_args(5, ARGS);
    struct simple_flags actual_flags = {0};
    parse_simple_flags(&parser, &actual_flags);
    assert(!has_args(&parser));
    assert_flags_equal(&expected_flags, &actual_flags);
}

int main(int argc, char *argv[]) {
    #ifdef NDEBUG
        #error "Can't set NDEBUG"
    #endif
    printf("Running empty_args:\n");
    test_empty_args();
    printf("Running only_positional:\n");
    test_only_positional();
    printf("Running basic_flags:\n");
    test_basic_flags();
}

