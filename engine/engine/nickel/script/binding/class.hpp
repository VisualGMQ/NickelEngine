#pragma once

#include "nickel/common/singleton.hpp"
#include "nickel/refl/util/function_traits.hpp"
#include "nickel/refl/util/type_list.hpp"
#include "nickel/refl/util/variable_traits.hpp"
#include "nickel/script/binding/class_base.hpp"
#include "nickel/script/binding/class_id.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/traits.hpp"
#include "nickel/script/binding/value_wrapper.hpp"
#include "quickjs.h"

namespace nickel::script {

template <typename T>
class QJSClass : public QJSClassBase {
public:
    QJSClass(JSContext* ctx, const std::string& name)
        : m_context{ctx}, m_ctor{JS_UNDEFINED} {
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

        auto& ids = QJSClassIDManager<T>::GetOrGen();

        QJS_CALL(m_context,
                 JS_NewClass(JS_GetRuntime(m_context), ids.m_id, &m_def));
        QJS_CALL(m_context, JS_NewClass(JS_GetRuntime(m_context),
                                        ids.m_const_id, &m_const_type_def));
        QJS_CALL(m_context, JS_NewClass(JS_GetRuntime(m_context),
                                        ids.m_pointer_id, &m_pointer_def));
        QJS_CALL(m_context,
                 JS_NewClass(JS_GetRuntime(m_context), ids.m_const_pointer_id,
                             &m_const_pointer_def));
        QJS_CALL(m_context, JS_NewClass(JS_GetRuntime(m_context), ids.m_ref_id,
                                        &m_ref_def));
        QJS_CALL(m_context, JS_NewClass(JS_GetRuntime(m_context),
                                        ids.m_const_ref_id, &m_const_ref_def));

        JS_SetClassProto(m_context, ids.m_id, m_proto);
        JS_SetClassProto(m_context, ids.m_const_id, m_const_proto);
        JS_SetClassProto(m_context, ids.m_pointer_id, m_pointer_proto);
        JS_SetClassProto(m_context, ids.m_const_pointer_id,
                         m_const_pointer_proto);
        JS_SetClassProto(m_context, ids.m_ref_id, m_ref_proto);
        JS_SetClassProto(m_context, ids.m_const_ref_id, m_const_ref_proto);
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
            QJS_CALL(m_context, JS_DefinePropertyValueStr(m_context, m_proto,
                                                          name.c_str(), fn, 0));
            QJS_CALL(m_context, JS_DefinePropertyValueStr(
                                    m_context, m_pointer_proto, name.c_str(),
                                    JS_DupValue(m_context, fn), 0));

            QJS_CALL(m_context, JS_DefinePropertyValueStr(
                                    m_context, m_ref_proto, name.c_str(),
                                    JS_DupValue(m_context, fn), 0));

            if constexpr (traits::is_const) {
                JSValue const_fn = JS_NewCFunction2(
                    m_context, JSMemberFnTraits<F>::Fn, name.c_str(),
                    refl::list_size_v<typename traits::args>, JS_CFUNC_generic,
                    0);
                QJS_CALL(m_context,
                         JS_DefinePropertyValueStr(m_context, m_const_proto,
                                                   name.c_str(), const_fn, 0));
                QJS_CALL(m_context,
                         JS_DefinePropertyValueStr(
                             m_context, m_const_ref_proto, name.c_str(),
                             JS_DupValue(m_context, const_fn), 0));
                QJS_CALL(m_context,
                         JS_DefinePropertyValueStr(
                             m_context, m_const_pointer_proto, name.c_str(),
                             JS_DupValue(m_context, const_fn), 0));
            }
        } else {
            JSValue fn = JS_NewCFunction2(
                m_context, JSFnTraits<F>::Fn, name.c_str(),
                refl::list_size_v<typename traits::args>, JS_CFUNC_generic, 0);
            QJS_CALL(m_context,
                     JS_SetPropertyStr(m_context, m_ctor, name.c_str(), fn));
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
            QJS_CALL(m_context,
                     JS_DefineProperty(
                         m_context, m_proto, name_atom, JS_UNDEFINED,
                         getter_value, JS_UNDEFINED,
                         JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
            QJS_CALL(m_context,
                     JS_DefineProperty(
                         m_context, m_ref_proto, name_atom, JS_UNDEFINED,
                         getter_value, JS_UNDEFINED,
                         JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
            QJS_CALL(m_context,
                     JS_DefineProperty(
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
            QJS_CALL(m_context,
                     JS_DefineProperty(m_context, m_proto, name_atom,
                                       JS_UNDEFINED, getter_value, setter_value,
                                       JS_PROP_C_W_E | JS_PROP_HAS_GET |
                                           JS_PROP_HAS_SET | JS_PROP_GETSET));
            QJS_CALL(m_context,
                     JS_DefineProperty(m_context, m_pointer_proto, name_atom,
                                       JS_UNDEFINED, getter_value, setter_value,
                                       JS_PROP_C_W_E | JS_PROP_HAS_GET |
                                           JS_PROP_HAS_SET | JS_PROP_GETSET));
            QJS_CALL(m_context,
                     JS_DefineProperty(m_context, m_ref_proto, name_atom,
                                       JS_UNDEFINED, getter_value, setter_value,
                                       JS_PROP_C_W_E | JS_PROP_HAS_GET |
                                           JS_PROP_HAS_SET | JS_PROP_GETSET));

            JS_FreeValue(m_context, setter_value);
        }

        QJS_CALL(m_context,
                 JS_DefineProperty(
                     m_context, m_const_proto, name_atom, JS_UNDEFINED,
                     getter_value, JS_UNDEFINED,
                     JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
        QJS_CALL(m_context,
                 JS_DefineProperty(
                     m_context, m_const_pointer_proto, name_atom, JS_UNDEFINED,
                     getter_value, JS_UNDEFINED,
                     JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));
        QJS_CALL(m_context,
                 JS_DefineProperty(
                     m_context, m_const_ref_proto, name_atom, JS_UNDEFINED,
                     getter_value, JS_UNDEFINED,
                     JS_PROP_C_W_E | JS_PROP_HAS_GET | JS_PROP_GETSET));

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

private:
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

}  // namespace nickel::script