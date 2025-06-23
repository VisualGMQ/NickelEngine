#pragma once

struct JSContext;

namespace nickel::script {

void LogJSException(JSContext*);

#define QJS_CALL(ctx, expr)                     \
    do {                                        \
        if ((expr) == -1) {                     \
            LOGE("qjs call failed: {}", #expr); \
            LogJSException(ctx);                \
        }                                       \
    } while (0)

#define JS_VALUE_CHECK(ctx, value)                      \
    do {                                                \
        if (JS_IsException(value)) LogJSException(ctx); \
    } while (0)

#define JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, value) \
    do {                                            \
        if (JS_IsException(value)) {                \
            LogJSException(ctx);                    \
            return JS_UNDEFINED;                    \
        }                                           \
    } while (0)

}  // namespace nickel::script