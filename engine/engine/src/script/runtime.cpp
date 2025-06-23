#pragma once
#include "nickel/script/binding/runtime.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/script/binding/context.hpp"
#include "quickjs.h"

namespace nickel::script {

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

}  // namespace nickel::script