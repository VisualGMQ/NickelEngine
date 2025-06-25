#include "nickel/script/binding/common.hpp"
#include "nickel/common/log.hpp"
#include "quickjs.h"

namespace nickel::script {

void LogJSException(JSContext* ctx) {
    JSValue exception = JS_GetException(ctx);

    const char* error = JS_ToCString(ctx, exception);
    if (error) {
        LOGE("JS Error: {}\n", error);
        JS_FreeCString(ctx, error);
    }

    JSValue stack = JS_GetPropertyStr(ctx, exception, "stack");
    if (!JS_IsUndefined(stack)) {
        const char* stack_str = JS_ToCString(ctx, stack);
        if (stack_str) {
            LOGE("Stack Trace:\n{}\n", stack_str);
            JS_FreeCString(ctx, stack_str);
        }
    }
    
    JS_FreeValue(ctx, stack);
    JS_FreeValue(ctx, exception);
}


}