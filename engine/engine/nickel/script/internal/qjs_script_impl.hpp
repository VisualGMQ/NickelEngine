#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/script/binding/context.hpp"
#include "quickjs.h"

namespace nickel::script {

class QuickJSScriptImpl : public RefCountable {
public:
    QuickJSScriptImpl(QJSContext&, JSValue);
    ~QuickJSScriptImpl();

    void OnUpdate();
    void DecRefcount() override;

    JSValue GetJSValue() const { return m_value; }

private:
    QJSContext& m_ctx;
    JSValue m_value;
};

}  // namespace nickel::script