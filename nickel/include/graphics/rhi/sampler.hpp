#pragma once

#include "graphics/rhi/common.hpp"
#include <optional>

namespace nickel::rhi {

class SamplerImpl;
class DeviceImpl;

class Sampler {
public:
    struct Descriptor final {
        SamplerAddressMode u = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode v = SamplerAddressMode::ClampToEdge;
        SamplerAddressMode w = SamplerAddressMode::ClampToEdge;
        std::optional<CompareOp> compare;
        uint32_t lodMinClamp = 0;
        uint32_t lodMaxClamp = 32;
        std::optional<float> maxAnisotropy;
        Filter min = Filter::Linear;
        Filter mag = Filter::Linear;
        Filter mipmapFilter = Filter::Linear;
    };

    Sampler() = default;
    explicit Sampler(APIPreference api, DeviceImpl& dev,
                     const Descriptor& desc);

    Sampler(Sampler&& o) noexcept { swap(o, *this); }
    Sampler(const Sampler& o) = default;
    Sampler& operator=(const Sampler& o) = default;

    Sampler& operator=(Sampler&& o) noexcept {
        if (&o != this) swap(o, *this);
        return *this;
    }
    void Destroy();

    operator bool() const {
        return impl_;
    }

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    SamplerImpl* impl_{};

    friend void swap(Sampler& o1, Sampler& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}