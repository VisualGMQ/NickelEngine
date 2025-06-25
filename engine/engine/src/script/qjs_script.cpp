#include "nickel/script/qjs_script.hpp"
#include "nickel/script/internal/qjs_script_impl.hpp"

namespace nickel::script {

void QuickJSScript::OnUpdate() {
    m_impl->OnUpdate();
}

}  // namespace nickel