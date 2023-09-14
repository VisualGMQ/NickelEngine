
#pragma once

#include <string>
#include <cassert>
#include "debugbreak.hpp"
#include "log.hpp"
#include "log_tag.hpp"

//! @addtogroup utilities
//! @{

//! @brief like c `assert()`. log fatal error message, and auto stop program if you are debugging
//! @param expr expression, be false will assert
//! @param msg  log fatal messge while assert
#define Assert(expr, msg) do { \
    if (!(expr)) { \
        LOGF("Assert", msg); \
        debug_break(); \
    } \
} while(0)

//! @}
