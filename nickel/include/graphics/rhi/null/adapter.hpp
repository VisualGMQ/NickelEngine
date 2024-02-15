#pragma once

#include "graphics/rhi/impl/adapter.hpp"

namespace nickel::rhi::null {

class AdapterImpl: public rhi::AdapterImpl {
public:
    GPUSupportFeatures Features() override;
    GPUSupportLimits Limits() override;
    Device RequestDevice() override;
    Adapter::Info RequestAdapterInfo() override;
};

}