/**
 * A C99 argparsing library, part of libidiot.
 * 
 * No documentation (yet), just read the source.
 * 
 * Dual-licensed under Creative Commons CC0 (Public Domain) and the MIT License.
 * 
 * Source code & issue tracker: https://github.com/Techcable/libidiot
 * 
 * VERSION: 0.1.0-beta.2
 *
 * CHANGELOG:
 * 0.1.0-beta.2:
 *    - Corrected support for positional arguments (and --)
 * 0.1.0-beta.1 - Initial release
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define _ARG_UNREACHABLE() do { \
        assert(false); \
        __builtin_unreachable(); \
    } while(false)

struct arg_parser {
    int idx;
    int argc;
    char** argv;
    char* current_value;
    bool finished_flags;
};
struct arg_config {
    // Short name for this argument
    char* short_name;
    // Full length aliases (null terminated),
    const char** aliases;
    // True if this argument is a flag, false if it is a value
    bool flag;
};

static inline bool has_args(struct arg_parser *parser) {
    return parser->idx < parser->argc;
}
// Forward declaration (because it actually has to do some amount of work)
static bool has_flag_args(struct arg_parser *parser);
static inline struct arg_parser init_args(int argc, char* argv[]) {
    struct arg_parser parser = {
        .idx = 1, // NOTE: Arg 0 is program name
        .argv = argv,
        .argc = argc,
        .current_value = NULL
    };
    return parser;
}
static inline char* consume_arg(struct arg_parser *parser) {
    assert(has_args(parser)); // Bounds check
    return parser->argv[parser->idx++];
}

static inline char *current_arg(struct arg_parser *parser) {
    if (has_args(parser)) {
        return parser->argv[parser->idx];
    } else {
        return NULL;
    }
}

static bool has_flag_args(struct arg_parser *parser) {
    if (!has_args(parser)) return false;
    if (parser->finished_flags) return false;
    char *arg = current_arg(parser);
    size_t current_arg_len = strlen(arg);
    switch (current_arg_len) {
        case 0:
        case 1:
            // NOTE: We used to give a warning for '-' as a positional arg.
            // Since neither `cargo pkgid -` or `git rev-parse -` do this
            // I have decided it is not unix-like and have decided
            // to unconditionally accept '-' (and the empty string)
            // as positional arguments
            goto finished_flags;
        case 2:
            // Three cases:
            // 1. arg == "-k" (short arg)
            // 2. arg == "--" (delimits the positional arguments)
            // 3. arg[0] != '-' (it's positional)
            if (arg[0] == '-') {
                if (arg[1] == '-') {
                    // case 2 - "--"
                    assert(memcmp(arg, "--", 2) == 0);
                    consume_arg(parser);
                    goto finished_flags;
                } else {
                    // case 1 - short arg
                    return true;
                }
            } else {
                // case 3 - positional
                goto finished_flags;
            }
        default:
            assert(current_arg_len >= 2);
            if (arg[0] == '-') {
                assert(strcmp(arg, "--") != 0);
                return true;
            } else {
                goto finished_flags;
            }
    }
finished_flags:
    parser->finished_flags = true;
    return false;
}

static const struct arg_config DEFAULT_CONFIG = {};
static bool match_arg(struct arg_parser *parser, const char *full_name, const struct arg_config *config) {
    assert(parser != NULL);
    assert(full_name != NULL);
    if (!has_flag_args(parser)) return false;
    if (config == NULL) config = &DEFAULT_CONFIG;
    char* arg = current_arg(parser);
    size_t len = strlen(arg);
    switch (len) {
        case 0:
        case 1:
            _ARG_UNREACHABLE(); // Should've been checked in has_flag_args
        case 2:
            // Both of these cases should've been cought earlier (in has_flag_args)
            assert(arg[0] == '-');
            assert(memcmp(arg, "--", 2) == 0);
            char actual_short = arg[1];
            if (config->short_name != NULL && actual_short == *config->short_name) {
                consume_arg(parser);
                goto matched_arg;
            } else {
                return false; // Some other flag (but not finished yet)
            }
            _ARG_UNREACHABLE();
        default:
            assert(len >= 3);
            if (memcmp(arg, "--", 2) == 0) {
                // Good, we have a flag
                char *actual_name = &arg[2];
                bool match = strcmp(actual_name, full_name) == 0;
                const char **aliases = config->aliases;
                if (aliases != NULL && !match) {
                    while (*aliases != NULL) {
                        const char *alias = *aliases;
                        if (strcmp(alias, actual_name) == 0) {
                            match = true;
                            break;
                        }
                        aliases += 1;
                    }
                }
                if (match) {
                    consume_arg(parser);
                    goto matched_arg;
                } else {
                    return false;
                }
            } else {
                if (arg[0] == '-') {
                    fprintf(stderr, "Long args must start with --name (not -name)");
                    fprintf(stderr, "Consider `--` as seperator if this is intended to be a poisitional arg");
                    exit(1);
                } else {
                    // Must be a positional value (which we should've cought earlier)
                    _ARG_UNREACHABLE();
                }
            }
    }
    _ARG_UNREACHABLE();
    matched_arg:
        parser->current_value = NULL;
        if (config->flag) {
            return true;
        }
        // Parse value
        if (has_args(parser)) {
            char* value = consume_arg(parser);
            assert(value != NULL);
            parser->current_value = value;
            return true;
        } else {
            fprintf(stderr, "ERROR: Expected a value for --%s\n", full_name);
            exit(1);
        }
}

#undef _ARG_UNREACHABLE // Macro hygine
