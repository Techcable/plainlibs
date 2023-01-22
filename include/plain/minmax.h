/**
 * Implements type-generic integer min/max operators.
 *
 * This module offers only two functions: plain_min and plain_max.
 *
 * They implement integer minimum & maximum respectively.
 *
 * Uses the C11 _Generic operator to select types based on first argument.
 *
 * LIMITATION: Types are currently selected based on the first argument,
 * so `plain_min((int) 7, (long) 7)` will be interpreted as
 * plain_min(int, int) and result in a type error.
 *
 * To workaround this, always ensure the largest integer type
 * is the first argument.
 *
 * ## Overriding the `min` and `max` names ("short names")
 * By default this module doesn't affect the macro names `min` or `max`.
 *
 * To override this behavior, define `PLAINLIBS_MINMAX_OVERRIDE_SHORT_NAMES`
 * before including the header.
 *
 * This will alias min/max macros to the plainlibs names, assuming they are not already .
 *
 * This will error if some other module (not plainlibs)
 *
 * Recomendation: Do not use the "short names" in API header files,
 * only in implementation files
 *
 * ### Undefining the "short names"
 * An alternative way to avoid namespace polution is to undefine the short names
 * after using them.
 *
 * This requires undefining all of `min`, `max`,
 * and `PLAINLIBS_MINMAX_SHORT_NAMES_ALREADY_DEFINED`.
 *
 * ## License, Source, Changelog
 *
 * Dual-licensed under Creative Commons CC0 (Public Domain) and the MIT License.
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
#ifndef PLAINLIBS_MINMAX_H
#define PLAINLIBS_MINMAX_H

#if !(__STDC_VERSION__ >= 201112L)
    #error "The minmax.h header requires C11"
#endif


#define _PLAIN_IMPL_MINMAX_INT(tp, prefix) \
    static inline tp _plain_ ## prefix ## max(tp a, tp b) { \
        return a > b ? a : b; \
    } \
    static inline tp _plain_ ## prefix ## min(tp a, tp b) { \
        return a < b ? a : b; \
    }

_PLAIN_IMPL_MINMAX_INT(char, c)
_PLAIN_IMPL_MINMAX_INT(unsigned char, uc)
_PLAIN_IMPL_MINMAX_INT(short, s)
_PLAIN_IMPL_MINMAX_INT(unsigned short, us)
_PLAIN_IMPL_MINMAX_INT(int, i)
_PLAIN_IMPL_MINMAX_INT(unsigned int, ui)
_PLAIN_IMPL_MINMAX_INT(long, l)
_PLAIN_IMPL_MINMAX_INT(unsigned long, ul)
_PLAIN_IMPL_MINMAX_INT(long long, ll)
_PLAIN_IMPL_MINMAX_INT(unsigned long long, ull)

/**
 * Take the maximum of the specified values.
 *
 * Only works for integer arguments.
 * 
 * Selects types based on the first argument,
 * so min((int) 7, (long) 7) will do integer-minimum and
 * result in a type error
 */
#define plain_max(a, b) _Generic((a), \
        char: _plain_cmax, \
        unsigned char: _plain_ucmax, \
        short: _plain_smax, \
        unsigned short: _plain_usmax, \
        int: _plain_imax, \
        unsigned int: _plain_uimax, \
        long: _plain_lmax, \
        unsigned long: _plain_ulmax, \
        long long: _plain_llmax, \
        unsigned long long: _plain_ullmax, \
        default: _plain_imax \
    )(a, b)

/**
 * Take the minimum of the specified values.
 *
 * Only works for integer arguments.
 * 
 * Selects types based on the first argument,
 * so min((int) 7, (long) 7) will do integer-minimum and
 * result in a type error
 */
#define plain_min(a, b) _Generic((a), \
        char: _plain_cmin, \
        unsigned char: _plain_ucmin, \
        short: _plain_smin, \
        unsigned short: _plain_usmin, \
        int: _plain_imin, \
        unsigned int: _plain_uimin, \
        long: _plain_lmin, \
        unsigned long: _plain_ulmin, \
        long long: _plain_llmin, \
        unsigned long long: _plain_ullmin, \
        default: _plain_imin \
    )(a, b)


#endif // PLAINLIBS_MINMAX_H

// Flags that we should override the standard 'min' and 'max' names
//
// This behavior is opt-in.
//
// It is outside include guards because it has side effects
#ifdef PLAINLIBS_MINMAX_OVERRIDE_SHORT_NAMES

#ifdef PLAINLIBS_MINMAX_SHORT_NAMES_ALREADY_DEFINED

#ifndef max
    #error "The `max` macro was marked as overriden by plainlibs, but is no longer defined. " \
           "Did you improperly undefine it?"
#elif !defined(min)
    #error "The `min` macro was marked as overriden by plainlibs, but is no longer defined. " \
           "Did you improperly undefine it?"
#endif

#elif defined(max) || defined(min)

#error "Either the `min` or `max` macros have already been defined outside of plainlibs. " \
       "This conflicts with the requested override by `PLAINLIBS_MINMAX_SHORT_NAMES` flag."

#else

#define max(a, b) plain_max(a, b)
#define min(a, b) plain_min(a, b)
#define PLAINLIBS_MINMAX_SHORT_NAMES_ALREADY_DEFINED

#endif

// Cancel the request, it has been fulfilled
#undef PLAINLIBS_MINMAX_OVERRIDE_SHORT_NAMES

#endif
