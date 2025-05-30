#pragma once
#include "nickel/common/impl_wrapper.hpp"

namespace nickel::script {
class QuickJSScriptImpl;

class QuickJSScript : public ImplWrapper<QuickJSScriptImpl> {
public:
    using ImplWrapper::ImplWrapper;

    void OnUpdate();
};

}  // namespace nickel