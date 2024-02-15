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
        Filter min = Filter::Nearest;
        Filter mag = Filter::Nearest;
        Filter mipmapFilter = Filter::Nearest;
    };

    Sampler() = default;
    explicit Sampler(APIPreference api, DeviceImpl& dev, const Descriptor& desc);
    void Destroy();

    operator bool() const {
        return impl_;
    }

    auto Impl() const { return impl_; }
    auto Impl() { return impl_; }

private:
    SamplerImpl* impl_{};
};

}