
#pragma once

#include "log.hpp"
#include "log_tag.hpp"
#include "stdpch.hpp"

namespace nickel {

template <typename... Args>
void doAssert(bool b, Args&&... args) {
    if (!b) {
        LOGF("Assert", std::forward<Args>(args)...);
        assert(b);
    }
}

/**
 * @addtogroup utilities
 * @{
 */

/**
 * @brief like c `assert()`. log fatal error message, and auto stop program if
 * you are debugging
 * @param expr expression, be false will assert
 * @param msg  log fatal messge while assert
 */
#define Assert(expr, ...)                              \
    do {                                               \
        ::nickel::doAssert(bool(expr), ##__VA_ARGS__); \
    } while (0)

/**
 * @brief tag a code not finish, run this macro will break app and step into
 * debug mode
 */
#define TODO(msg) Assert(false, msg)

}  // namespace nickel

/**
 * @}
 */