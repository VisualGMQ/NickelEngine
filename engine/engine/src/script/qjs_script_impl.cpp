#include "nickel/script/internal/qjs_script_impl.hpp"

#include "nickel/script/internal/script_impl.hpp"

namespace nickel::script {

QuickJSScriptImpl::QuickJSScriptImpl(ScriptManagerImpl& mgr,QJSContext& ctx, JSValue value)
    : m_ctx{ctx}, m_manager{mgr}, m_value{value} {}

QuickJSScriptImpl::~QuickJSScriptImpl() {
    JS_FreeValue(m_ctx, m_value);
}

void QuickJSScriptImpl::OnUpdate() {
    // TODO: tick script
}

void QuickJSScriptImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_manager.m_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::script