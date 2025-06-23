#include "nickel/script/internal/qjs_script_impl.hpp"

namespace nickel::script {

QuickJSScriptImpl::QuickJSScriptImpl(QJSContext& ctx, JSValue value)
    : m_ctx{ctx}, m_value{value} {}

QuickJSScriptImpl::~QuickJSScriptImpl() {
    JS_FreeValue(m_ctx, m_value);
}

void QuickJSScriptImpl::OnUpdate() {
    // TODO: not finish
}

void QuickJSScriptImpl::DecRefcount() {
    RefCountable::DecRefcount();

    // FIXME: temporary for test
    if (Refcount() == 0) {
        delete this;
    }
}

}  // namespace nickel::script