#include "nickel/script/binding/module.hpp"
#include "nickel/common/assert.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/context.hpp"
#include "nickel/script/binding/runtime.hpp"
#include "quickjs-libc.h"
#include "quickjs.h"

namespace nickel::script {
QJSModule::QJSModule(QJSContext& context, const std::string& name)
    : m_context{context}, m_name{name} {
    NICKEL_ASSERT(context);
    m_module = JS_NewCModule(context, name.c_str(), moduleInitFunc);
    NICKEL_RETURN_IF_FALSE_LOGE(m_module, "create qjs module failed");
}

QJSContext& QJSModule::EndModule() const {
    if (m_context && m_module) {
        for (auto& property : m_properties) {
            QJS_CALL(m_context, JS_AddModuleExport(m_context, m_module,
                                                   property.m_name.c_str()));
        }

        for (auto& clazz : m_classes) {
            QJS_CALL(m_context, JS_AddModuleExport(m_context, m_module,
                                                   clazz->GetName().c_str()));
            QJS_CALL(m_context,
                     JS_AddModuleExport(m_context, m_module,
                                        clazz->GetConstName().c_str()));
            QJS_CALL(m_context,
                     JS_AddModuleExport(m_context, m_module,
                                        clazz->GetRefName().c_str()));
            QJS_CALL(m_context,
                     JS_AddModuleExport(m_context, m_module,
                                        clazz->GetConstRefName().c_str()));
            QJS_CALL(m_context,
                     JS_AddModuleExport(m_context, m_module,
                                        clazz->GetPointerName().c_str()));
            QJS_CALL(m_context,
                     JS_AddModuleExport(m_context, m_module,
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