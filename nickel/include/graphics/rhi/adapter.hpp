#pragma once

#include "graphics/rhi/device.hpp"
#include "graphics/rhi/command.hpp"
#include "stdpch.hpp"

namespace nickel::rhi {

struct GPUSupportFeatures {
};

struct GPUSupportLimits {
    uint32_t minUniformBufferOffsetAlignment = 256;
    uint32_t minStorageBufferOffsetAlignment;
    uint32_t nonCoherentAtomSize = 0;
    uint32_t maxPushConstantSize = 0;
    bool supportGeometryShader;
};

APIPreference GetSupportRenderAPI(APIPreference desired);

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

    Adapter(Adapter&& o) { swap(*this, o); }
    Adapter(const Adapter& o) = default;
    Adapter& operator=(const Adapter& o) = default;

    Adapter& operator=(Adapter&& o) noexcept {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    GPUSupportFeatures Features();
    const GPUSupportLimits& Limits() const;
    Device RequestDevice();
    Info RequestAdapterInfo() const;
    void Destroy();

    auto Impl() const { return impl_; }

    auto Impl() { return impl_; }

private:
    AdapterImpl* impl_{};

    friend void swap(Adapter& o1, Adapter& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}  // namespace nickel::rhi