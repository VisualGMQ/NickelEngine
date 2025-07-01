#pragma once
#include "nickel/script/binding/runtime.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/script/binding/class_factory.hpp"
#include "nickel/script/binding/context.hpp"
#include "quickjs.h"

#include <quickjs-libc.h>

namespace nickel::script {

QJSRuntime::QJSRuntime() {
    m_runtime = JS_NewRuntime();
    JS_SetRuntimeOpaque(m_runtime, this);
    NICKEL_RETURN_IF_FALSE_LOGE(m_runtime,
                                "Failed to create JS runtime object");

    m_context = std::make_unique<QJSContext>(*this);

    m_class_factory = std::make_unique<QJSClassFactory>();
    m_id_manager = std::make_unique<QJSClassIDManager>(m_runtime);
    m_id_family_manager =
        std::make_unique<QJSClassIDFamilyManager>(m_runtime, *m_id_manager);
}

QJSRuntime::~QJSRuntime() {
    m_class_factory.reset();
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

QJSClassFactory& QJSRuntime::GetClassFactory() {
    return *m_class_factory;
}

QJSClassIDManager& QJSRuntime::GetClassIDManager() {
    return *m_id_manager;
}

QJSClassIDFamilyManager& QJSRuntime::GetClassIDFamilyManager() {
    return *m_id_family_manager;
}

}  // namespace nickel::script