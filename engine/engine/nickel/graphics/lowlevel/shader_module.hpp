#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/impl_wrapper.hpp"

namespace nickel::graphics {

class ShaderModuleImpl;

class NICKEL_API ShaderModule: public ImplWrapper<ShaderModuleImpl> {
public:
    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics