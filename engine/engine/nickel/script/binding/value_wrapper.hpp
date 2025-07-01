#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/script/binding/class_id.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/context.hpp"
#include "quickjs.h"

#include <concepts>

namespace nickel::script {

template <typename T>
struct JSValueWrapper;

template <PureClass T>
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSClassID id =
            QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx)).m_id;
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        // FIXME: maybe copy in ?
        QJS_CALL(ctx, JS_SetOpaque(obj, ::new(std::nothrow) T{value}));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(JS_GetOpaque(
            value,
            QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx)).m_id));
    }
};

template <PureClass T>
struct JSValueWrapper<const T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSClassID id =
            QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx)).m_const_id;
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, ::new (std::nothrow) T{value}));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(JS_GetOpaque(
            value, QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                       .m_const_id));
    }
};

template <PureClass T>
struct JSValueWrapper<const T*> {
    JSValue Wrap(JSContext* ctx, const T* value) {
        JSClassID id = QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                           .m_const_pointer_id;
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, (void*)value));
        return obj;
    }

    const T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<const T*>(JS_GetOpaque(
            value, QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                       .m_const_pointer_id));
    }
};

template <PureClass T>
struct JSValueWrapper<T*> {
    JSValue Wrap(JSContext* ctx, T* value) {
        JSClassID id = QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                           .m_pointer_id;
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, (void*)value));
        return obj;
    }

    T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<T*>(JS_GetOpaque(
            value, QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                       .m_pointer_id));
    }
};

template <PureClass T>
struct JSValueWrapper<T&> {
    JSValue Wrap(JSContext* ctx, T& value) {
        JSClassID id =
            QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx)).m_ref_id;
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(JS_GetOpaque(
            value,
            QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx)).m_ref_id));
    }
};

template <PureClass T>
struct JSValueWrapper<const T&> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        JSClassID id = QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                           .m_const_ref_id;
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    const T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<const T*>(JS_GetOpaque(
            value, QJSClassIDFamilyManager<T>::GetOrGen(JS_GetRuntime(ctx))
                       .m_const_ref_id));
    }
};

template <typename T>
requires std::is_enum_v<T>
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        using underlying_type = std::underlying_type_t<T>;
        JSValueWrapper<underlying_type> underlying_wrapper;
        return underlying_wrapper.Wrap(ctx,
                                       static_cast<underlying_type>(value));
    }

    T Unwrap(JSContext* ctx, const JSValue& value) {
        using underlying_type = std::underlying_type_t<T>;
        JSValueWrapper<underlying_type> underlying_wrapper;
        return static_cast<T>(underlying_wrapper.Unwrap(ctx, value));
    }
};

template <typename T>
struct JSValueWrapper<T*> {
    JSValue Wrap(JSContext* ctx, T* value) {
        JSValue obj = JS_NewObjectClass(
            ctx, QJSClassIDManager<T*>::GetOrGen(JS_GetRuntime(ctx)));
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, value));
        return obj;
    }

    T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<T*>(JS_GetOpaque(
            value, QJSClassIDManager<T*>::GetOrGen(JS_GetRuntime(ctx))));
    }
};

template <typename T>
struct JSValueWrapper<const T&> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        JSValue obj = JS_NewObjectClass(
            ctx, QJSClassIDManager<const T&>::GetOrGen(JS_GetRuntime(ctx)));
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, &value));
        return obj;
    }

    const T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<const T*>(JS_GetOpaque(
            value, QJSClassIDManager<const T&>::GetOrGen(JS_GetRuntime(ctx))));
    }
};

template <typename T>
struct JSValueWrapper<T&> {
    JSValue Wrap(JSContext* ctx, T& value) {
        JSValue obj = JS_NewObjectClass(
            ctx, QJSClassIDManager<T&>::GetOrGen(JS_GetRuntime(ctx)));
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, &value));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(JS_GetOpaque(
            value, QJSClassIDManager<T&>::GetOrGen(JS_GetRuntime(ctx))));
    }
};

template <typename T>
struct JSValueWrapper<const T> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        JSValue obj = JS_NewObjectClass(
            ctx, QJSClassIDManager<const T>::GetOrGen(JS_GetRuntime(ctx)));
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    const T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<const T*>(JS_GetOpaque(
            value, QJSClassIDManager<const T>::GetOrGen(JS_GetRuntime(ctx))));
    }
};

template <typename T>
struct JSValueWrapper<const T*> {
    JSValue Wrap(JSContext* ctx, const T* value) {
        JSValue obj = JS_NewObjectClass(
            ctx, QJSClassIDManager<const T*>::GetOrGen(JS_GetRuntime(ctx)));
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(ctx, JS_SetOpaque(obj, value));
        return obj;
    }

    const T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<const T*>(JS_GetOpaque(
            value, QJSClassIDManager<const T*>::GetOrGen(JS_GetRuntime(ctx))
                       .m_const_pointer_id));
    }
};

template <typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T> && sizeof(T) <= 4)
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSValue js_value = JS_NewUint32(ctx, value);
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    T Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsNumber(value),
                                          "js value is not number");

        uint32_t num;
        QJS_CALL(ctx, JS_ToUint32(ctx, &num, value));

        return num;
    }
};

template <typename T>
requires(std::is_integral_v<T> && std::is_unsigned_v<T> && sizeof(T) > 4)
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSValue js_value = JS_NewBigUint64(ctx, value);
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    T Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsNumber(value),
                                          "js value is not number");

        uint64_t num;
        QJS_CALL(ctx, JS_ToBigUint64(ctx, &num, value));

        return num;
    }
};

template <typename T>
requires(std::is_integral_v<T> && !std::is_unsigned_v<T> && sizeof(T) <= 4)
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSValue js_value = JS_NewInt32(ctx, value);
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    T Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsNumber(value),
                                          "js value is not number");

        int32_t num;
        QJS_CALL(ctx, JS_ToInt32(ctx, &num, value));

        return num;
    }
};

template <typename T>
requires(std::is_integral_v<T> && !std::is_unsigned_v<T> && sizeof(T) > 4)
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSValue js_value = JS_NewBigInt64(ctx, value);
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    T Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsNumber(value),
                                          "js value is not number");

        int64_t num;
        QJS_CALL(ctx, JS_ToBigInt64(ctx, &num, value));

        return num;
    }
};

template <std::floating_point T>
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T value) {
        JSValue js_value = JS_NewFloat64(ctx, value);
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    T Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsNumber(value),
                                          "js value is not number");

        double num;
        QJS_CALL(ctx, JS_ToFloat64(ctx, &num, value));

        return num;
    }
};

template <>
struct JSValueWrapper<bool> {
    JSValue Wrap(JSContext* ctx, bool value) {
        JSValue js_value = JS_NewBool(ctx, value);
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    bool Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsBool(value),
                                          "js value is not boolean");

        int ret = JS_ToBool(ctx, value);
        if (ret == -1) {
            LogJSException(ctx);
        }

        return ret != 0;
    }
};

template <>
struct JSValueWrapper<const bool> {
    JSValue Wrap(JSContext* ctx, bool value) {
        return JSValueWrapper<bool>{}.Wrap(ctx, value);
    }

    bool Unwrap(JSContext* ctx, const JSValue& value) {
        return JSValueWrapper<bool>{}.Unwrap(ctx, value);
    }
};

template <>
struct JSValueWrapper<const float> {
    JSValue Wrap(JSContext* ctx, float value) {
        return JSValueWrapper<float>{}.Wrap(ctx, value);
    }

    float Unwrap(JSContext* ctx, const JSValue& value) {
        return JSValueWrapper<float>{}.Unwrap(ctx, value);
    }
};

template <>
struct JSValueWrapper<const double> {
    JSValue Wrap(JSContext* ctx, double value) {
        return JSValueWrapper<double>{}.Wrap(ctx, value);
    }

    double Unwrap(JSContext* ctx, const JSValue& value) {
        return JSValueWrapper<double>{}.Unwrap(ctx, value);
    }
};

// TODO: move string & string_view to class binding
template <>
struct JSValueWrapper<std::string_view> {
    JSValue Wrap(JSContext* ctx, std::string_view str) {
        JSValue js_value = JS_NewStringLen(ctx, str.data(), str.size());
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    std::string_view Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsString(value),
                                          "js value is not string");

        const char* str = JS_ToCString(ctx, value);
        if (!str) {
            LogJSException(ctx);
        }

        return str;
    }
};

template <>
struct JSValueWrapper<std::string> {
    JSValue Wrap(JSContext* ctx, const std::string& str) {
        JSValue js_value = JS_NewStringLen(ctx, str.data(), str.size());
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    std::string Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsString(value),
                                          "js value is not string");

        const char* str = JS_ToCString(ctx, value);
        if (!str) {
            LogJSException(ctx);
        }

        return str;
    }
};

template <>
struct JSValueWrapper<const char*> {
    JSValue Wrap(JSContext* ctx, const char* str) {
        JSValue js_value = JS_NewStringLen(ctx, str, strlen(str));
        JS_VALUE_CHECK_RETURN_UNDEFINED(ctx, js_value);
        return js_value;
    }

    std::string Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE({}, JS_IsString(value),
                                          "js value is not string");

        const char* str = JS_ToCString(ctx, value);
        if (!str) {
            LogJSException(ctx);
        }

        return str;
    }
};

template <>
struct JSValueWrapper<const int> {
    JSValue Wrap(JSContext* ctx, int value) {
        JSValueWrapper<int> wrapper{};
        return wrapper.Wrap(ctx, value);
    }

    int Unwrap(JSContext* ctx, const JSValue& value) {
        JSValueWrapper<int> wrapper{};
        return wrapper.Unwrap(ctx, value);
    }
};

template <>
struct JSValueWrapper<const char> {
    JSValue Wrap(JSContext* ctx, char value) {
        JSValueWrapper<char> wrapper{};
        return wrapper.Wrap(ctx, value);
    }

    char Unwrap(JSContext* ctx, const JSValue& value) {
        JSValueWrapper<char> wrapper{};
        return wrapper.Unwrap(ctx, value);
    }
};

template <>
struct JSValueWrapper<const short> {
    JSValue Wrap(JSContext* ctx, short value) {
        JSValueWrapper<short> wrapper{};
        return wrapper.Wrap(ctx, value);
    }

    short Unwrap(JSContext* ctx, const JSValue& value) {
        JSValueWrapper<short> wrapper{};
        return wrapper.Unwrap(ctx, value);
    }
};

template <>
struct JSValueWrapper<const long> {
    JSValue Wrap(JSContext* ctx, long value) {
        JSValueWrapper<long> wrapper{};
        return wrapper.Wrap(ctx, value);
    }

    long Unwrap(JSContext* ctx, const JSValue& value) {
        JSValueWrapper<long> wrapper{};
        return wrapper.Unwrap(ctx, value);
    }
};

template <>
struct JSValueWrapper<const long long> {
    JSValue Wrap(JSContext* ctx, long long value) {
        JSValueWrapper<long long> wrapper{};
        return wrapper.Wrap(ctx, value);
    }

    long long Unwrap(JSContext* ctx, const JSValue& value) {
        JSValueWrapper<long long> wrapper{};
        return wrapper.Unwrap(ctx, value);
    }
};

}  // namespace nickel::script