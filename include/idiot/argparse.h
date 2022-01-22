/**
 * A C99 argparsing library, designed for idiots.
 * 
 * No documentation, just read the source.
 * 
 * Licensed under Creative Commons CC0 (Public Domain).
 * 
 * VERSION: 0.1.0-beta.1
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define UNREACHABLE() do { \
        assert(false); \
        __builtin_unreachable(); \
    } while(false)

struct arg_parser {
    int idx;
    int argc;
    char** argv;
    char* current_value;
    bool finished;
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
static inline bool has_flag_args(struct arg_parser *parser) {
    return !parser->finished && has_args(parser);
}
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
            if (strcmp(arg, "-")) {
                fprintf(stderr, "Encountered `-` without a short option\n");
                fprintf(stderr, "Consider `--` as seperator if this is intended to be a poisitional arg\n");
            }
            return false;
        case 2:
            if (arg[0] == '-') {
                if (arg[1] == '-') {
                    assert(memcmp(arg, "--", 2));
                    // Signals end of flags
                    consume_arg(parser);
                    goto finished_flags;
                }
                char actual_short = arg[1];
                if (config->short_name != NULL && actual_short == *config->short_name) {
                    consume_arg(parser);
                    goto matched_arg;
                } else {
                    return false; // Some other flag (but not finished yet)
                }
            } else {
                // Must be a positional arg.
                goto finished_flags;
            }
            UNREACHABLE();
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
                }
                // Must be a positional value 
                goto finished_flags;
            }
    }
    UNREACHABLE();
    finished_flags:
        // Mark as finished
        parser->finished = true;
        return false;
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

#undef UNREACHABLE // Macro hygine