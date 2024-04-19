#pragma once

#include "graphics/rhi/impl/adapter.hpp"

namespace nickel::rhi::null {

class AdapterImpl: public rhi::AdapterImpl {
public:
    GPUSupportFeatures Features() override;
    const GPUSupportLimits& Limits() const override;
    Device RequestDevice() override;
    Adapter::Info RequestAdapterInfo() const override;

private:
    GPUSupportLimits limits_;
};

}