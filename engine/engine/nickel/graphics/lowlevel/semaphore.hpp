#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/impl_wrapper.hpp"

namespace nickel::graphics {

class SemaphoreImpl;

class NICKEL_API Semaphore: public ImplWrapper<SemaphoreImpl> {
public:
    using ImplWrapper::ImplWrapper;
};

}