#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/refl/util/type_list.hpp"
#include "nickel/script/binding/class_id.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/value_wrapper.hpp"
#include "quickjs.h"

namespace nickel::script {

template <typename>
class QJSClass;

template <typename T>
struct FnParamConverter {
    static T Convert(JSContext* ctx, JSValue value) {
        return JSValueWrapper<T>{}.Unwrap(ctx, value);
    }
};

template <PureClass T>
struct FnParamConverter<T&> {
    static T& Convert(JSContext* ctx, JSValue value) {
        auto& id_family_manager =
            static_cast<QJSRuntime*>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)))
                ->GetClassIDFamilyManager();
        auto ids = id_family_manager.GetOrGen<T>();
        auto id = JS_GetClassID(value);
        if (id == ids.m_id || id == ids.m_ref_id || id == ids.m_pointer_id) {
            return *static_cast<T*>(JS_GetOpaque(value, id));
        }

        NICKEL_ASSERT(false, "[quickjs]: invalid argument convert");
    }
};

template <PureClass T>
struct FnParamConverter<T*> {
    static T* Convert(JSContext* ctx, JSValue value) {
        auto& id_family_manager =
            static_cast<QJSRuntime*>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)))
                ->GetClassIDFamilyManager();
        auto ids = id_family_manager.GetOrGen<T>();
        auto id = JS_GetClassID(value);
        if (id == ids.m_id || id == ids.m_ref_id || id == ids.m_pointer_id) {
            return static_cast<T*>(JS_GetOpaque(value, id));
        }

        NICKEL_ASSERT(false, "[quickjs]: invalid argument convert");
    }
};

template <PureClass T>
struct FnParamConverter<const T*> {
    static const T* Convert(JSContext*, JSValue value) {
        return static_cast<T*>(JS_GetOpaque(value, JS_GetClassID(value)));
    }
};

template <PureClass T>
struct FnParamConverter<const T&> {
    static const T& Convert(JSContext*, JSValue value) {
        return *static_cast<T*>(JS_GetOpaque(value, JS_GetClassID(value)));
    }
};

template <typename Class, typename... Args>
struct JSConstructorTraits {
    static JSValue Fn(JSContext* ctx, JSValueConst new_target, int argc,
                      JSValueConst* argv) {
        NICKEL_ASSERT(argc == sizeof...(Args));

        Class* p = callConstructor(ctx, argv,
                                   std::make_index_sequence<sizeof...(Args)>{});

        auto& id_family_manager =
            static_cast<QJSRuntime*>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)))
                ->GetClassIDFamilyManager();
        auto id = id_family_manager.GetOrGen<Class>().m_id;
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
            FnParamConverter<refl::list_element_t<args_list, Indices>>::Convert(
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
            auto&& return_value =
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

        auto& id_family_manager =
            static_cast<QJSRuntime*>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)))
                ->GetClassIDFamilyManager();
        auto ids = id_family_manager.GetOrGen<clazz>();

        if (id == ids.m_id) {
            clazz* self_obj = static_cast<clazz*>(JS_GetOpaque(self, id));
            return doCallFn(ctx, self_obj, argv, indices);
        }
        if (id == ids.m_const_id) {
            const clazz* self_obj =
                static_cast<const clazz*>(JS_GetOpaque(self, id));
            return doCallFn(ctx, self_obj, argv, indices);
        }
        if (id == ids.m_ref_id) {
            clazz* self_obj = static_cast<clazz*>(JS_GetOpaque(self, id));
            return doCallFn(ctx, self_obj, argv, indices);
        }
        if (id == ids.m_const_ref_id) {
            const clazz& self_obj =
                *static_cast<const clazz*>(JS_GetOpaque(self, id));
            return doCallFn(ctx, &self_obj, argv, indices);
        }
        if (id == ids.m_pointer_id) {
            clazz* self_obj = static_cast<clazz*>(JS_GetOpaque(self, id));
            return doCallFn(ctx, self_obj, argv, indices);
        }
        if (id == ids.m_const_pointer_id) {
            const clazz* self_obj =
                static_cast<const clazz*>(JS_GetOpaque(self, id));
            return doCallFn(ctx, self_obj, argv, indices);
        }
    }

    template <typename T, size_t... Indices>
    static return_type doCallFn(JSContext* ctx, T&& self, JSValue* argv,
                                std::index_sequence<Indices...>) {
        if constexpr (sizeof...(Indices) == 0) {
            return std::invoke(F, std::forward<T>(self));
        } else {
            return std::invoke(
                F, std::forward<T>(self),
                FnParamConverter<refl::list_element_t<args_list, Indices>>::
                    Convert(ctx, argv[Indices])...);
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
            auto&& return_value =
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
            FnParamConverter<refl::list_element_t<args_list, Indices>>::Convert(
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

        auto& id_family_manager =
            static_cast<QJSRuntime*>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)))
                ->GetClassIDFamilyManager();
        auto ids = id_family_manager.GetOrGen<clazz>();
        NICKEL_ASSERT(id == ids.m_id || id == ids.m_const_id);
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

        auto& id_family_manager =
            static_cast<QJSRuntime*>(JS_GetRuntimeOpaque(JS_GetRuntime(ctx)))
                ->GetClassIDFamilyManager();

        clazz* p = static_cast<clazz*>(
            JS_GetOpaque2(ctx, this_val, id_family_manager.GetOrGen<clazz>().m_id));
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