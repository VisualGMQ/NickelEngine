#pragma once
#include "nickel/common/log.hpp"

#define NICKEL_RETURN_IF_FALSE(expr) \
    do {                             \
        if (!(expr)) return;         \
    } while (0)

#define NICKEL_CONTINUE_IF_FALSE(expr) \
    if (!(expr)) continue;

#define NICKEL_BREAK_IF_FALSE(expr) \
    if (!(expr)) break;

#define NICKEL_RETURN_IF_FALSE_LOGE(expr, msg, ...)     \
    do {                                                \
        if (!(expr)) {                                  \
            LOGE(#expr " failed: " msg, ##__VA_ARGS__); \
            return;                                     \
        }                                               \
    } while (0)

#define NICKEL_RETURN_IF_FALSE_LOGI(expr, msg, ...)     \
    do {                                                \
        if (!(expr)) {                                  \
            LOGI(#expr " failed: " msg, ##__VA_ARGS__); \
            return;                                     \
        }                                               \
    } while (0)

#define NICKEL_RETURN_IF_FALSE_LOGW(expr, msg, ...)     \
    do {                                                \
        if (!(expr)) {                                  \
            LOGW(#expr " failed: " msg, ##__VA_ARGS__); \
            return;                                     \
        }                                               \
    } while (0)

#define NICKEL_RETURN_VALUE_IF_FALSE_LOGW(value, expr, msg, ...) \
    do {                                                         \
        if (!(expr)) {                                           \
            LOGW(#expr " failed: " msg, ##__VA_ARGS__);          \
            return value;                                        \
        }                                                        \
    } while (0)

#define NICKEL_RETURN_VALUE_IF_FALSE_LOGE(value, expr, msg, ...) \
    do {                                                         \
        if (!(expr)) {                                           \
            LOGE(#expr " failed: " msg, ##__VA_ARGS__);          \
            return value;                                        \
        }                                                        \
    } while (0)