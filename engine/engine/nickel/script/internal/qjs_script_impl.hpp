#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/script/binding/context.hpp"
#include "quickjs.h"

namespace nickel::script {

class ScriptManagerImpl;

class QuickJSScriptImpl : public RefCountable {
public:
    QuickJSScriptImpl(ScriptManagerImpl&, QJSContext&, JSValue);
    ~QuickJSScriptImpl();

    void OnUpdate();
    void DecRefcount() override;

    JSValue GetJSValue() const { return m_value; }

private:
    QJSContext& m_ctx;
    ScriptManagerImpl& m_manager;
    JSValue m_value;
};

}  // namespace nickel::script