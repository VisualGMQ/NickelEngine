#pragma once
#include "nickel/common/log.hpp"

namespace nickel {

// Ref:
// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros

#define _NICKEL_ASSERT_1_ARGS(x) \
    if (!(x)) {                  \
        LOGC("{}", #x);          \
    }

#define _NICKEL_ASSERT_2_ARGS(x, msg) \
    if (!(x)) {                       \
        LOGC("{}: {}", #x, msg);      \
    }

#define _NICKEL_GET_NTH_ARGS(arg1, arg2, arg3, ...) arg3
#define _NICKEL_ASSERT_CHOOSER(...)                          \
    _NICKEL_GET_NTH_ARGS(__VA_ARGS__, _NICKEL_ASSERT_2_ARGS, \
                         _NICKEL_ASSERT_1_ARGS)

#ifdef NICKEL_DEBUG
/**
 * @brief assert macro, has two forms:
 *  1. NICKEL_ASSERT(condition) will assert and log
 *  2. NICKEL_ASSERT(condition, msg) will assert and log additional msg
 */
#define NICKEL_ASSERT(...)                               \
    do {                                                 \
        _NICKEL_ASSERT_CHOOSER(__VA_ARGS__)(__VA_ARGS__) \
    } while (0)

#define NICKEL_CANT_REACH() NICKEL_ASSERT(false, "won't reach here")
#else
#define NICKEL_ASSERT(...)
#define NICKEL_CANT_REACH(msg)
#endif

}  // namespace nickel