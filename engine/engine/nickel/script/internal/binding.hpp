#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/common/log.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/fs/path.hpp"
#include "nickel/refl/util/function_traits.hpp"
#include "nickel/refl/util/misc.hpp"
#include "nickel/refl/util/type_list.hpp"
#include "nickel/refl/util/variable_traits.hpp"
#include "nickel/script/internal/common.hpp"
#include "quickjs.h"

#include <cstdint>

namespace nickel::script {

#define QJS_CALL(expr)                                          \
    do {                                                        \
        if ((expr) == -1) {                                     \
            LOGE("qjs call failed: {}", #expr);                 \
            LogJSException(QJSRuntime::GetInst().GetContext()); \
        }                                                       \
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

template <typename T>
class QJSClass;

class QJSRuntime;
class QJSModule;

class QJSContext {
public:
    explicit QJSContext(QJSRuntime& runtime);
    ~QJSContext();

    JSValue Eval(std::span<const char> content, const Path& filename,
                 bool strict_mode) const;

    QJSModule& NewModule(const std::string& name);

    auto& GetModules() const { return m_modules; }

    QJSRuntime& GetRuntime() const;

    operator JSContext*() const;
    operator bool() const;

private:
    JSContext* m_context{};
    QJSRuntime& m_runtime;

    std::vector<std::unique_ptr<QJSModule>> m_modules;
};

class QJSRuntime : public Singlton<QJSRuntime, false> {
public:
    QJSRuntime();
    ~QJSRuntime();

    operator JSRuntime*() const;

    const QJSContext& GetContext() const;
    QJSContext& GetContext();

private:
    JSRuntime* m_runtime{};
    std::unique_ptr<QJSContext> m_context;
};

class QJSClassBase;

template <typename T>
struct JSValueWrapper;

template <typename T>
requires std::is_class_v<T>
struct JSValueWrapper<T> {
    JSValue Wrap(JSContext* ctx, T& value) {
        JSClassID id = QJSClass<T>::GetID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        // FIXME: maybe copy in ?
        QJS_CALL(JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(JS_GetOpaque(value, QJSClass<T>::GetID()));
    }
};

template <typename T>
requires std::is_class_v<T>
struct JSValueWrapper<const T> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        // FIXME: maybe copy in ?
        JSClassID id = QJSClass<T>::GetConstTypeID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(
            JS_GetOpaque(value, QJSClass<T>::GetConstTypeID()));
    }
};

template <typename T>
requires std::is_class_v<T>
struct JSValueWrapper<const T*> {
    JSValue Wrap(JSContext* ctx, const T* value) {
        JSClassID id = QJSClass<T>::GetConstPointerTypeID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, (void*)value));
        return obj;
    }

    const T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<const T*>(
            JS_GetOpaque(value, QJSClass<T>::GetConstPointerTypeID()));
    }
};

template <typename T>
requires std::is_class_v<T>
struct JSValueWrapper<T*> {
    JSValue Wrap(JSContext* ctx, T* value) {
        JSClassID id = QJSClass<std::remove_const_t<T>>::GetPointerTypeID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, (void*)value));
        return obj;
    }

    T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<T*>(
            JS_GetOpaque(value, QJSClass<T>::GetPointerTypeID()));
    }
};

template <typename T>
requires std::is_class_v<T>
struct JSValueWrapper<T&> {
    JSValue Wrap(JSContext* ctx, T& value) {
        JSClassID id = QJSClass<T>::GetRefTypeID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(
            JS_GetOpaque(value, QJSClass<T>::GetRefTypeID()));
    }
};

template <typename T>
requires std::is_class_v<T>
struct JSValueWrapper<const T&> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        JSClassID id = QJSClass<T>::GetConstRefTypeID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    const T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<const T*>(
            JS_GetOpaque(value, QJSClass<T>::GetConstRefTypeID()));
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
        JSValue obj = JS_NewObjectClass(ctx, QJSClass<T>::GetPointerTypeID());
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, value));
        return obj;
    }

    T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<T*>(
            JS_GetOpaque(value, QJSClass<T>::GetPointerTypeID()));
    }
};

template <typename T>
struct JSValueWrapper<const T&> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        JSValue obj = JS_NewObjectClass(ctx, QJSClass<T>::GetConstRefTypeID());
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, &value));
        return obj;
    }

    const T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<const T*>(
            JS_GetOpaque(value, QJSClass<T>::GetConstRefTypeID()));
    }
};

template <typename T>
struct JSValueWrapper<T&> {
    JSValue Wrap(JSContext* ctx, T& value) {
        JSValue obj = JS_NewObjectClass(ctx, QJSClass<T>::GetRefTypeID());
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, &value));
        return obj;
    }

    T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<T*>(
            JS_GetOpaque(value, QJSClass<T>::GetRefTypeID()));
    }
};

template <typename T>
struct JSValueWrapper<const T> {
    JSValue Wrap(JSContext* ctx, const T& value) {
        JSValue obj = JS_NewObjectClass(ctx, QJSClass<T>::GetConstTypeID());
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, (void*)&value));
        return obj;
    }

    const T& Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_ASSERT(JS_IsObject(value), "js value is not object");
        return *static_cast<const T*>(
            JS_GetOpaque(value, QJSClass<T>::GetConstTypeID()));
    }
};

template <typename T>
struct JSValueWrapper<const T*> {
    JSValue Wrap(JSContext* ctx, const T* value) {
        JSValue obj =
            JS_NewObjectClass(ctx, QJSClass<T>::GetConstPointerTypeID());
        if (JS_IsException(obj)) {
            LogJSException(ctx);
        }
        QJS_CALL(JS_SetOpaque(obj, value));
        return obj;
    }

    const T* Unwrap(JSContext* ctx, const JSValue& value) {
        NICKEL_RETURN_VALUE_IF_FALSE_LOGE(nullptr, JS_IsObject(value),
                                          "js value is not object");
        return static_cast<const T*>(
            JS_GetOpaque(value, QJSClass<T>::GetConstPointerTypeID()));
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
        QJS_CALL(JS_ToUint32(ctx, &num, value));

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
        QJS_CALL(JS_ToBigUint64(ctx, &num, value));

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
        QJS_CALL(JS_ToInt32(ctx, &num, value));

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
        QJS_CALL(JS_ToBigInt64(ctx, &num, value));

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
        QJS_CALL(JS_ToFloat64(ctx, &num, value));

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

class QJSClassBase {
public:
    QJSClassBase() = default;
    QJSClassBase(const QJSClassBase&) = delete;
    QJSClassBase& operator=(const QJSClassBase&) = delete;

    virtual void EndClass() = 0;
    virtual JSValue GetConstructor() const = 0;
    virtual ~QJSClassBase() = default;
    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetConstName() const = 0;
    virtual const std::string& GetRefName() const = 0;
    virtual const std::string& GetConstRefName() const = 0;
    virtual const std::string& GetPointerName() const = 0;
    virtual const std::string& GetConstPointerName() const = 0;
};

class QJSClassFactory : public Singlton<QJSClassFactory, false> {
public:
    template <typename T>
    QJSClass<T>& CreateOrGet(JSContext* ctx, const std::string& name) {
        static JSClassID id = 0;
        static JSClassID const_id = 0;
        static JSClassID pointer_id = 0;
        static JSClassID const_pointer_id = 0;
        static JSClassID ref_id = 0;
        static JSClassID const_ref_id = 0;

        if (id != 0) {
            return static_cast<QJSClass<T>&>(*m_mutable_map[id]);
        }

        if (const_id != 0) {
            return static_cast<QJSClass<T>&>(*m_const_map[id]);
        }

        if (pointer_id != 0) {
            return static_cast<QJSClass<T>&>(*m_pointer_map[id]);
        }

        if (const_pointer_id != 0) {
            return static_cast<QJSClass<T>&>(*m_const_pointer_map[id]);
        }

        if (ref_id != 0) {
            return static_cast<QJSClass<T>&>(*m_ref_map[id]);
        }

        if (const_ref_id != 0) {
            return static_cast<QJSClass<T>&>(*m_const_ref_map[id]);
        }

        id = JS_NewClassID(JS_GetRuntime(ctx), &id);
        const_id = JS_NewClassID(JS_GetRuntime(ctx), &const_id);
        pointer_id = JS_NewClassID(JS_GetRuntime(ctx), &pointer_id);
        const_pointer_id = JS_NewClassID(JS_GetRuntime(ctx), &const_pointer_id);
        ref_id = JS_NewClassID(JS_GetRuntime(ctx), &ref_id);
        const_ref_id = JS_NewClassID(JS_GetRuntime(ctx), &const_ref_id);

        auto& qjs_class = m_classes.emplace_back(std::make_unique<QJSClass<T>>(
            ctx, id, const_id, pointer_id, const_pointer_id, ref_id,
            const_ref_id, name));

        QJSClassBase* ptr = qjs_class.get();
        m_mutable_map.emplace(id, ptr);
        m_const_map.emplace(const_id, ptr);
        m_pointer_map.emplace(pointer_id, ptr);
        m_const_pointer_map.emplace(const_pointer_id, ptr);
        m_ref_map.emplace(ref_id, ptr);
        m_const_ref_map.emplace(const_ref_id, ptr);

        return static_cast<QJSClass<T>&>(*ptr);
    }

    void DoRegister() const {
        for (auto& clazz : m_classes) {
            clazz->EndClass();
        }
    }

private:
    std::vector<std::unique_ptr<QJSClassBase>> m_classes;

    std::unordered_map<JSClassID, QJSClassBase*> m_mutable_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_const_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_pointer_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_const_pointer_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_ref_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_const_ref_map;
};

class QJSModule {
public:
    QJSModule(QJSContext&, const std::string& name);

    QJSContext& EndModule() const;

    template <typename T>
    QJSModule& AddProperty(const std::string& name, const T& value) {
        JSValueWrapper<T> wrapper;
        JSValue js_value = wrapper.Wrap(m_context, value);
        if (!JS_IsUndefined(js_value)) {
            m_properties.push_back({name, js_value});
        }
        return *this;
    }

    template <typename T>
    QJSClass<T>& AddClass(const std::string& name) {
        auto& qjs_class =
            QJSClassFactory::GetInst().CreateOrGet<T>(m_context, name);
        qjs_class.ResetClassName(name);
        return static_cast<QJSClass<T>&>(*m_classes.emplace_back(&qjs_class));
    }

    const std::string& GetName() const;

private:
    struct Property {
        std::string m_name;
        JSValue m_value;
    };

    QJSContext& m_context;
    std::string m_name;
    JSModuleDef* m_module{};
    std::vector<Property> m_properties;
    std::vector<QJSClassBase*> m_classes;

    static int moduleInitFunc(JSContext* ctx, JSModuleDef* m);
};

template <typename Class, typename... Args>
struct JSConstructorTraits {
    static JSValue Fn(JSContext* ctx, JSValueConst new_target, int argc,
                      JSValueConst* argv) {
        NICKEL_ASSERT(argc == sizeof...(Args));

        Class* p = callConstructor(ctx, argv,
                                   std::make_index_sequence<sizeof...(Args)>{});

        auto id = QJSClass<Class>::GetID();
        JSValue obj = JS_NewObjectClass(ctx, id);
        if (JS_IsException(obj)) {
            LogJSException(ctx);
            return JS_UNDEFINED;
        }
        QJS_CALL(JS_SetOpaque(obj, p));
        return obj;
    }

private:
    template <size_t... Indices>
    static Class* callConstructor(JSContext* ctx, JSValueConst* argv,
                                  std::index_sequence<Indices...>) {
        using args_list = refl::type_list<Args...>;
        return new Class{
            JSValueWrapper<refl::list_element_t<args_list, Indices>>{}.Unwrap(
                ctx, argv[Indices])...};
    }
};

template <auto F>
struct JSMemberFnTraits {
    using traits = refl::function_pointer_traits<F>;
    static_assert(traits::is_member);

    using clazz = typename traits::clazz;
    using args_list = typename traits::args;
    using return_type = typename traits::return_type;
    static constexpr size_t args_num = refl::list_size_v<args_list>;

    static JSValue Fn(JSContext* ctx, JSValueConst self, int argc,
                      JSValueConst* argv) {
        NICKEL_ASSERT(argc == refl::list_size_v<args_list>);

        if constexpr (std::is_void_v<return_type>) {
            callFn(ctx, self, argv, std::make_index_sequence<args_num>{});
            return JS_UNDEFINED;
        } else {
            auto return_value =
                callFn(ctx, self, argv, std::make_index_sequence<args_num>{});
            JSValueWrapper<return_type> wrapper;
            return wrapper.Wrap(ctx, return_value);
        }
    }

private:
    template <size_t... Indices>
    static return_type callFn(JSContext* ctx, JSValue self, JSValue* argv,
                              std::index_sequence<Indices...> indices) {
        JSClassID id = JS_GetClassID(self);
        if (id == 0) {
            NICKEL_ASSERT("call function on unregistered class");
        }

        if (id == QJSClass<clazz>::GetID()) {
            clazz* self_obj = static_cast<clazz*>(JS_GetOpaque(self, id));
            return doCallFn(self_obj, argv, indices);
        }
        if (id == QJSClass<clazz>::GetConstTypeID()) {
            const clazz* self_obj =
                static_cast<const clazz*>(JS_GetOpaque(self, id));
            return doCallFn(self_obj, argv, indices);
        }
        if (id == QJSClass<clazz>::GetRefTypeID()) {
            clazz& self_obj = *static_cast<clazz*>(JS_GetOpaque(self, id));
            return doCallFn(&self_obj, argv, indices);
        }
        if (id == QJSClass<clazz>::GetConstRefTypeID()) {
            const clazz& self_obj =
                *static_cast<const clazz*>(JS_GetOpaque(self, id));
            return doCallFn(&self_obj, argv, indices);
        }
        if (id == QJSClass<clazz>::GetPointerTypeID()) {
            clazz* self_obj = static_cast<clazz*>(JS_GetOpaque(self, id));
            return doCallFn(self_obj, argv, indices);
        }
        if (id == QJSClass<clazz>::GetConstPointerTypeID()) {
            const clazz* self_obj =
                static_cast<const clazz*>(JS_GetOpaque(self, id));
            return doCallFn(self_obj, argv, indices);
        }
    }

    template <typename T, size_t... Indices>
    static return_type doCallFn(T&& self, JSValue* argv,
                                std::index_sequence<Indices...>) {
        if constexpr (sizeof...(Indices) == 0) {
            return std::invoke(F, std::forward<T>(self));
        } else {
            return std::invoke(
                F, std::forward<T>(self),
                JSValueWrapper<refl::list_element_t<args_list, Indices>>{}
                    .Unwrap(argv[Indices])...);
        }
    }
};

template <auto F>
struct JSFnTraits {
    using traits = refl::function_pointer_traits<F>;
    static_assert(!traits::is_member);

    using args_list = typename traits::args;
    using return_type = typename traits::return_type;
    static constexpr size_t args_num = refl::list_size_v<args_list>;

    static JSValue Fn(JSContext* ctx, JSValueConst self, int argc,
                      JSValueConst* argv) {
        NICKEL_ASSERT(argc == refl::list_size_v<args_list>);

        if constexpr (std::is_void_v<return_type>) {
            callFn(ctx, argv, std::make_index_sequence<args_num>{});
            return JS_UNDEFINED;
        } else {
            auto return_value =
                callFn(ctx, argv, std::make_index_sequence<args_num>{});
            JSValueWrapper<return_type> wrapper;
            return wrapper.Wrap(ctx, return_value);
        }
    }

private:
    template <size_t... Indices>
    static return_type callFn(JSContext* ctx, JSValueConst* argv,
                              std::index_sequence<Indices...>) {
        return std::invoke(
            F,
            JSValueWrapper<refl::list_element_t<args_list, Indices>>{}.Unwrap(
                ctx, argv[Indices])...);
    }
};

template <auto F>
struct JSMemberVariableTraits {
    using traits = refl::variable_pointer_traits<F>;
    using clazz = typename traits::clazz;
    using type = typename traits::type;
    static_assert(traits::is_member);

    static JSValue Getter(JSContext* ctx, JSValue this_val) {
        JSClassID id = JS_GetClassID(this_val);

        NICKEL_ASSERT(id == QJSClass<clazz>::GetID() ||
                      id == QJSClass<clazz>::GetConstTypeID());
        const clazz* p =
            static_cast<const clazz*>(JS_GetOpaque2(ctx, this_val, id));
        if (!p) {
            LOGE("access class variable from nullptr");
            return JS_EXCEPTION;
        }

        auto value = std::invoke(F, p);
        return JSValueWrapper<type>{}.Wrap(ctx, value);
    }

    static JSValue Setter(JSContext* ctx, JSValueConst this_val,
                          JSValueConst argv) {
        JSClassID id = JS_GetClassID(this_val);
        if (id == 0) {
            return JS_ThrowTypeError(ctx, "operate on unregistered cpp class");
        }

        clazz* p = static_cast<clazz*>(
            JS_GetOpaque2(ctx, this_val, QJSClass<clazz>::GetID()));
        if (!p) {
            LOGE("access class variable from nullptr");
            return JS_EXCEPTION;
        }

        JSValueWrapper<type> wrapper{};
        auto& value = std::invoke(F, p);
        value = wrapper.Unwrap(ctx, argv);
        return JS_UNDEFINED;
    }
};

template <typename T>
class QJSClass : public QJSClassBase {
public:
    QJSClass(JSContext* ctx, JSClassID id, JSClassID const_id,
             JSClassID pointer_id, JSClassID const_pointer_id, JSClassID ref_id,
             JSClassID const_ref_id, const std::string& name)
        : m_context{ctx}, m_ctor{JS_UNDEFINED} {
        QJSClass::id = id;
        QJSClass::const_id = const_id;
        QJSClass::pointer_id = pointer_id;
        QJSClass::const_pointer_id = const_pointer_id;
        QJSClass::ref_id = ref_id;
        QJSClass::const_ref_id = const_ref_id;

        ResetClassName(name);

        m_proto = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_proto);
        m_const_proto = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_const_proto);
        m_pointer_proto = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_pointer_proto);
        m_const_pointer_proto = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_const_pointer_proto);
        m_ref_proto = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_ref_proto);
        m_const_ref_proto = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_const_ref_proto);
    }

    void ResetClassName(const std::string& name) {
        m_name = name;
        m_const_name = "const " + name;
        m_pointer_name = name + "*";
        m_const_pointer_name = "const " + name + "*";
        m_ref_name = name + "&";
        m_const_ref_name = "const " + name + "&";
    }

    void EndClass() override {
        m_def.class_name = m_name.c_str();
        m_def.finalizer = jsFinalizer;

        m_const_type_def.class_name = m_const_name.c_str();
        m_pointer_def.class_name = m_pointer_name.c_str();
        m_const_pointer_def.class_name = m_const_pointer_name.c_str();
        m_ref_def.class_name = m_ref_name.c_str();
        m_const_ref_def.class_name = m_const_ref_name.c_str();

        QJS_CALL(JS_NewClass(JS_GetRuntime(m_context), id, &m_def));
        QJS_CALL(
            JS_NewClass(JS_GetRuntime(m_context), const_id, &m_const_type_def));
        QJS_CALL(
            JS_NewClass(JS_GetRuntime(m_context), pointer_id, &m_pointer_def));
        QJS_CALL(JS_NewClass(JS_GetRuntime(m_context), const_pointer_id,
                             &m_const_pointer_def));
        QJS_CALL(JS_NewClass(JS_GetRuntime(m_context), ref_id, &m_ref_def));
        QJS_CALL(JS_NewClass(JS_GetRuntime(m_context), const_ref_id,
                             &m_const_ref_def));

        JS_SetClassProto(m_context, id, m_proto);
        JS_SetClassProto(m_context, const_id, m_const_proto);
        JS_SetClassProto(m_context, pointer_id, m_pointer_proto);
        JS_SetClassProto(m_context, const_pointer_id, m_const_pointer_proto);
        JS_SetClassProto(m_context, ref_id, m_ref_proto);
        JS_SetClassProto(m_context, const_ref_id, m_const_ref_proto);
    }

    template <typename... Args>
    QJSClass& AddConstructor() {
        if (!JS_IsUndefined(m_ctor)) {
            LOGW("Constructor already exists. Now we don't support function "
                 "overload");
            return *this;
        }

        m_ctor = JS_NewCFunction2(
            m_context, JSConstructorTraits<T, Args...>::Fn, m_name.c_str(),
            sizeof...(Args), JS_CFUNC_constructor, 0);
        if (JS_IsException(m_ctor)) {
            LogJSException(m_context);
        } else {
            JS_SetConstructor(m_context, m_ctor, m_proto);
        }
        return *this;
    }

    template <auto F>
    QJSClass& AddFunction(const std::string& name) {
        using traits = refl::function_pointer_traits<F>;
        if constexpr (traits::is_member) {
            JSValue fn = JS_NewCFunction2(
                m_context, JSMemberFnTraits<F>::Fn, name.c_str(),
                refl::list_size_v<typename traits::args>, JS_CFUNC_generic, 0);
            QJS_CALL(JS_DefinePropertyValueStr(m_context, m_proto, name.c_str(),
                                               fn, 0));
            QJS_CALL(JS_DefinePropertyValueStr(m_context, m_pointer_proto,
                                               name.c_str(),
                                               JS_DupValue(m_context, fn), 0));

            QJS_CALL(JS_DefinePropertyValueStr(m_context, m_ref_proto,
                                               name.c_str(),
                                               JS_DupValue(m_context, fn), 0));

            if constexpr (traits::is_const) {
                JSValue const_fn = JS_NewCFunction2(
                    m_context, JSMemberFnTraits<F>::Fn, name.c_str(),
                    refl::list_size_v<typename traits::args>, JS_CFUNC_generic,
                    0);
                QJS_CALL(JS_DefinePropertyValueStr(m_context, m_const_proto,
                                                   name.c_str(), const_fn, 0));
                QJS_CALL(JS_DefinePropertyValueStr(
                    m_context, m_const_ref_proto, name.c_str(),
                    JS_DupValue(m_context, const_fn), 0));
                QJS_CALL(JS_DefinePropertyValueStr(
                    m_context, m_const_pointer_proto, name.c_str(),
                    JS_DupValue(m_context, const_fn), 0));
            }
        } else {
            JSValue fn = JS_NewCFunction2(
                m_context, JSFnTraits<F>::Fn, name.c_str(),
                refl::list_size_v<typename traits::args>, JS_CFUNC_generic, 0);
            QJS_CALL(JS_SetPropertyStr(m_context, m_ctor, name.c_str(), fn));
        }
        return *this;
    }

    template <auto F>
    QJSClass& AddField(const std::string& name) {
        using traits = refl::variable_pointer_traits<F>;
        static_assert(traits::is_member);
        using js_traits = JSMemberVariableTraits<F>;

        JSCFunctionType getter;
        getter.getter = js_traits::Getter;
        JSValue getter_value =
            JS_NewCFunction2(m_context, getter.generic,
                             ("getter " + name).c_str(), 0, JS_CFUNC_getter, 0);
        JS_VALUE_CHECK(m_context, getter_value);
        JSAtom name_atom = JS_NewAtom(m_context, name.c_str());
        if constexpr (std::is_const_v<typename traits::type>) {
            QJS_CALL(JS_DefineProperty(
                m_context, m_proto, name_atom, JS_UNDEFINED, getter_value,
                JS_UNDEFINED,
                JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
            QJS_CALL(JS_DefineProperty(
                m_context, m_ref_proto, name_atom, JS_UNDEFINED, getter_value,
                JS_UNDEFINED,
                JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
            QJS_CALL(JS_DefineProperty(
                m_context, m_pointer_proto, name_atom, JS_UNDEFINED,
                getter_value, JS_UNDEFINED,
                JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
        } else {
            JSCFunctionType setter;
            setter.setter = js_traits::Setter;
            JSValue setter_value = JS_NewCFunction2(m_context, setter.generic,
                                                    ("setter " + name).c_str(),
                                                    1, JS_CFUNC_setter, 0);
            JS_VALUE_CHECK(m_context, setter_value);
            QJS_CALL(JS_DefineProperty(m_context, m_proto, name_atom,
                                       JS_UNDEFINED, getter_value, setter_value,
                                       JS_PROP_C_W_E | JS_PROP_HAS_GET |
                                           JS_PROP_HAS_SET | JS_PROP_GETSET));
            QJS_CALL(JS_DefineProperty(m_context, m_pointer_proto, name_atom,
                                       JS_UNDEFINED, getter_value, setter_value,
                                       JS_PROP_C_W_E | JS_PROP_HAS_GET |
                                           JS_PROP_HAS_SET | JS_PROP_GETSET));
            QJS_CALL(JS_DefineProperty(m_context, m_ref_proto, name_atom,
                                       JS_UNDEFINED, getter_value, setter_value,
                                       JS_PROP_C_W_E | JS_PROP_HAS_GET |
                                           JS_PROP_HAS_SET | JS_PROP_GETSET));

            JS_FreeValue(m_context, setter_value);
        }

        QJS_CALL(JS_DefineProperty(
            m_context, m_const_proto, name_atom, JS_UNDEFINED, getter_value,
            JS_UNDEFINED, JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
        QJS_CALL(JS_DefineProperty(
            m_context, m_const_pointer_proto, name_atom, JS_UNDEFINED,
            getter_value, JS_UNDEFINED,
            JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
        QJS_CALL(JS_DefineProperty(
            m_context, m_const_ref_proto, name_atom, JS_UNDEFINED, getter_value,
            JS_UNDEFINED, JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));

        JS_FreeValue(m_context, getter_value);
        JS_FreeAtom(m_context, name_atom);

        return *this;
    }

    template <typename U>
    QJSClass& AddStaticField(const std::string& name, U* ptr) {
        JS_SetPropertyStr(m_context, m_ctor, name.c_str(),
                          JSValueWrapper<U>{}.Wrap(m_context, *ptr));
        return *this;
    }

    JSValue GetConstructor() const override { return m_ctor; }

    const std::string& GetName() const override { return m_name; }

    const std::string& GetConstName() const override { return m_const_name; }

    const std::string& GetRefName() const override { return m_ref_name; }

    const std::string& GetConstRefName() const override {
        return m_const_ref_name;
    }

    const std::string& GetPointerName() const override {
        return m_pointer_name;
    }

    const std::string& GetConstPointerName() const override {
        return m_const_pointer_name;
    }

    static JSClassID GetID() { return id; }

    static JSClassID GetConstTypeID() { return const_id; }

    static JSClassID GetRefTypeID() { return ref_id; }

    static JSClassID GetConstRefTypeID() { return const_ref_id; }

    static JSClassID GetPointerTypeID() { return pointer_id; }

    static JSClassID GetConstPointerTypeID() { return const_pointer_id; }

private:
    static JSClassID id;
    static JSClassID const_id;
    static JSClassID pointer_id;
    static JSClassID const_pointer_id;
    static JSClassID ref_id;
    static JSClassID const_ref_id;

    JSContext* m_context;

    JSClassDef m_def{};
    JSClassDef m_const_type_def{};
    JSClassDef m_pointer_def{};
    JSClassDef m_const_pointer_def{};
    JSClassDef m_ref_def{};
    JSClassDef m_const_ref_def{};

    std::string m_name;
    std::string m_const_name;
    std::string m_ref_name;
    std::string m_const_ref_name;
    std::string m_pointer_name;
    std::string m_const_pointer_name;

    JSValue m_proto;
    JSValue m_const_proto;
    JSValue m_ref_proto;
    JSValue m_const_ref_proto;
    JSValue m_pointer_proto;
    JSValue m_const_pointer_proto;

    JSValue m_ctor;

    static void jsFinalizer(JSRuntime* rt, JSValue val) {
        T* p = static_cast<T*>(JS_GetOpaque(val, JS_GetClassID(val)));
        if (p) delete p;
    }
};

template <typename T>
JSClassID QJSClass<T>::id{};

template <typename T>
JSClassID QJSClass<T>::const_id{};

template <typename T>
JSClassID QJSClass<T>::pointer_id{};

template <typename T>
JSClassID QJSClass<T>::const_pointer_id{};

template <typename T>
JSClassID QJSClass<T>::ref_id{};

template <typename T>
JSClassID QJSClass<T>::const_ref_id{};

}  // namespace nickel::script