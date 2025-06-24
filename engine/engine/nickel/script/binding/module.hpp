#pragma once
#include "nickel/script/binding/enum.hpp"
#include "nickel/script/binding/class.hpp"
#include "nickel/script/binding/class_factory.hpp"

namespace nickel::script {

class QJSContext;

class QJSModule {
public:
    QJSModule(QJSContext&, QJSClassFactory&, const std::string& name);

    QJSContext& EndModule() const;

    template <typename T>
    QJSModule& AddField(const std::string& name, const T& value) {
        JSValueWrapper<T> wrapper;
        JSValue js_value = wrapper.Wrap(m_context, value);
        if (!JS_IsUndefined(js_value)) {
            m_properties.push_back({name, js_value});
        }
        return *this;
    }

    template <auto F>
    QJSModule& AddFunction(const std::string& name) {
        using fn_traits = JSFnTraits<F>;
        JSValue value = JS_NewCFunction(m_context, fn_traits::Fn, name.c_str(),
                                        fn_traits::args_num);
        if (JS_IsException(value)) {
            LogJSException(m_context);
            return *this;
        }
        m_properties.push_back({name, value});
        return *this;
    }

    template <typename T>
    QJSClass<T>& AddClass(const std::string& name) {
        auto& qjs_class = m_class_factory.CreateOrGet<T>(m_context, name);
        qjs_class.ResetClassName(name);
        qjs_class.SetOwnerModule(*this);
        return static_cast<QJSClass<T>&>(*m_classes.emplace_back(&qjs_class));
    }

    template <typename T>
    QJSEnum<T, QJSModule>& AddEnum(const std::string& name) {
        return static_cast<QJSEnum<T, QJSModule>&>(
            *m_enums.emplace_back(std::make_unique<QJSEnum<T, QJSModule>>(
                *this, m_context, name)));
    }

    const std::string& GetName() const;

private:
    struct Property {
        std::string m_name;
        JSValue m_value;
    };

    QJSClassFactory& m_class_factory;
    QJSContext& m_context;
    std::string m_name;
    JSModuleDef* m_module{};
    std::vector<Property> m_properties;
    std::vector<QJSClassBase*> m_classes;
    std::vector<std::unique_ptr<QJSEnumBase>> m_enums;

    static int moduleInitFunc(JSContext* ctx, JSModuleDef* m);
};

}  // namespace nickel::script