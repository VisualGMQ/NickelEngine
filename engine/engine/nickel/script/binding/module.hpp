#pragma once
#include "nickel/script/binding/class_factory.hpp"
#include "nickel/script/binding/class.hpp"

namespace nickel::script {

class QJSContext;

class QJSModule {
public:
    QJSModule(QJSContext&, QJSClassFactory&, const std::string& name);

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
        auto& qjs_class = m_class_factory.CreateOrGet<T>(m_context, name);
        qjs_class.ResetClassName(name);
        return static_cast<QJSClass<T>&>(*m_classes.emplace_back(&qjs_class));
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

    static int moduleInitFunc(JSContext* ctx, JSModuleDef* m);
};

}  // namespace nickel::script