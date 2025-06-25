#include "nickel/script/binding/context.hpp"
#include "nickel/common/log.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/module.hpp"
#include "nickel/script/binding/runtime.hpp"
#include "quickjs-libc.h"
#include "quickjs.h"

namespace nickel::script {

QJSContext::QJSContext(QJSRuntime& runtime) : m_runtime{runtime} {
    m_context = JS_NewContext(runtime);
    if (!m_context) {
        LOGE("Failed to create JS context");
    }

    js_std_add_helpers(m_context, 0, nullptr);
    js_init_module_bjson(m_context, "bjson");
    js_init_module_os(m_context, "os");
    js_init_module_std(m_context, "std");

    JS_SetContextOpaque(m_context, this);
}

QJSContext::~QJSContext() {
    m_modules.clear();
    JS_FreeContext(m_context);
}

JSValue QJSContext::Eval(std::span<const char> content, const Path& filename,
                         bool strict_mode) const {
    // FIXME: there must be a better way to auto import modules
    std::string module_import_code;
    for (auto& module : m_modules) {
        auto& name = module->GetName();
        module_import_code += "import * as " + name + " from '" + name + "'\n" +
                              "globalThis." + name + " = " + name + "\n";
    }

    {
        JSValue result =
            JS_Eval(m_context, module_import_code.data(),
                    module_import_code.size(), "<import>", JS_EVAL_TYPE_MODULE);
        JS_VALUE_CHECK_RETURN_UNDEFINED(m_context, result);
        JS_FreeValue(m_context, result);
    }

    JSValue value = JS_Eval(
        m_context, content.data(), content.size(), filename.ToString().c_str(),
        JS_EVAL_TYPE_GLOBAL | (strict_mode ? JS_EVAL_FLAG_STRICT : 0));
    JS_VALUE_CHECK_RETURN_UNDEFINED(m_context, value);
    return value;
}

QJSModule& QJSContext::NewModule(const std::string& name) {
    return *m_modules.emplace_back(
        std::make_unique<QJSModule>(*this, m_runtime.GetClassFactory(), name));
}

QJSRuntime& QJSContext::GetRuntime() const {
    return m_runtime;
}

QJSContext::operator JSContext*() const {
    return m_context;
}

QJSContext::operator bool() const {
    return m_context;
}

}  // namespace nickel::script