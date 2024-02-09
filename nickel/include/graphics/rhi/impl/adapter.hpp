#pragma once

#include "graphics/rhi/adapter.hpp"

namespace nickel::rhi {

class AdapterImpl {
public:
    virtual ~AdapterImpl() = default;

    virtual GPUSupportFeatures Features() = 0;
    virtual GPUSupportLimits Limits() = 0;
    virtual Device RequestDevice() = 0;
    virtual Adapter::Info RequestAdapterInfo() = 0;
};

}