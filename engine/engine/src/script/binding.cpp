#include "nickel/script/internal/binding.hpp"

#include "nickel/common/assert.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/context.hpp"
#include "nickel/script/internal/script_impl.hpp"

namespace nickel::script {

QJSContext::QJSContext(QJSRuntime& runtime) : m_runtime{runtime} {
    m_context = JS_NewContext(runtime);
    if (!m_context) {
        LOGE("Failed to create JS context");
    }
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

    JSValue value = JS_Eval(m_context, content.data(), content.size(),
                            filename.ToString().c_str(),
                            strict_mode ? JS_EVAL_FLAG_STRICT : 0);
    JS_VALUE_CHECK_RETURN_UNDEFINED(m_context, value);
    return value;
}

QJSModule& QJSContext::NewModule(const std::string& name) {
    return *m_modules.emplace_back(std::make_unique<QJSModule>(*this, name));
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

QJSRuntime::QJSRuntime() {
    m_runtime = JS_NewRuntime();
    NICKEL_RETURN_IF_FALSE_LOGE(m_runtime,
                                "Failed to create JS runtime object");

    m_context = std::make_unique<QJSContext>(*this);
}

QJSRuntime::~QJSRuntime() {
    m_context.reset();
    JS_FreeRuntime(m_runtime);
}

QJSRuntime::operator JSRuntime*() const {
    return m_runtime;
}

const QJSContext& QJSRuntime::GetContext() const {
    return *m_context;
}

QJSContext& QJSRuntime::GetContext() {
    return *m_context;
}

QJSModule::QJSModule(QJSContext& context, const std::string& name)
    : m_context{context}, m_name{name} {
    NICKEL_ASSERT(context);
    m_module = JS_NewCModule(context, name.c_str(), moduleInitFunc);
    NICKEL_RETURN_IF_FALSE_LOGE(m_module, "create qjs module failed");
}

QJSContext& QJSModule::EndModule() const {
    if (m_context && m_module) {
        for (auto& property : m_properties) {
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        property.m_name.c_str()));
        }

        for (auto& clazz : m_classes) {
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        clazz->GetName().c_str()));
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        clazz->GetConstName().c_str()));
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        clazz->GetRefName().c_str()));
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        clazz->GetConstRefName().c_str()));
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        clazz->GetPointerName().c_str()));
            QJS_CALL(JS_AddModuleExport(m_context, m_module,
                                        clazz->GetConstPointerName().c_str()));
        }
    }
    return m_context;
}

const std::string& QJSModule::GetName() const {
    return m_name;
}

int QJSModule::moduleInitFunc(JSContext* ctx, JSModuleDef* m) {
    for (auto& module : QJSRuntime::GetInst().GetContext().GetModules()) {
        if (module->m_module == m) {
            bool success = true;
            for (auto& property : module->m_properties) {
                if (JS_SetModuleExport(ctx, m, property.m_name.c_str(),
                                       property.m_value) == -1) {
                    LOGE("export {} from module {} failed!", property.m_name,
                         module->m_name);
                    success = false;
                }
            }
            for (auto& clazz : module->m_classes) {
                auto& name = clazz->GetName();
                if (JS_SetModuleExport(ctx, m, name.c_str(),
                                       clazz->GetConstructor()) == -1) {
                    LOGE("export class {} from module {} failed!", name,
                         module->m_name);
                    success = false;
                }
            }
            return success ? 0 : -1;
        }
    }

    return -1;
}

}  // namespace nickel::script