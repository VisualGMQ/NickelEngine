#pragma once

#include "graphics/rhi/device.hpp"
#include "graphics/rhi/command.hpp"
#include "stdpch.hpp"

namespace nickel::rhi {

struct GPUSupportFeatures {
};

struct GPUSupportLimits {
    uint32_t nonCoherentAtomSize = 0;
};

class AdapterImpl;

class Adapter final {
public:

    struct Option final {
        APIPreference api = APIPreference::Undefine;
    };

    struct Info final {
        APIPreference api = APIPreference::Undefine;
        std::string device;
        uint32_t vendorID;
    };

    Adapter(void* window, Option option = {APIPreference::Undefine});
    GPUSupportFeatures Features();
    const GPUSupportLimits& Limits() const;
    Device RequestDevice();
    Info RequestAdapterInfo();
    void Destroy();

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    AdapterImpl* impl_{};
};

}  // namespace nickel::rhi