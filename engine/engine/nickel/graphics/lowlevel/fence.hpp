#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/impl_wrapper.hpp"

namespace nickel::graphics {

class FenceImpl;

class NICKEL_API Fence: public ImplWrapper<FenceImpl> {
public:
    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics