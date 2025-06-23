#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/refl/util/type_list.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/value_wrapper.hpp"
#include "quickjs.h"

namespace nickel::script {

template <typename>
class QJSClass;

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
        QJS_CALL(ctx, JS_SetOpaque(obj, p));
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
            return JSValueWrapper<return_type>{}.Wrap(ctx, return_value);
        }
    }

private:
    template <size_t... Indices>
    static return_type callFn(JSContext* ctx, JSValue self, JSValue* argv,
                              std::index_sequence<Indices...> indices) {
        JSClassID id = JS_GetClassID(self);
        NICKEL_ASSERT(id != 0, "call function on unregistered class");

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

}  // namespace nickel::script