#pragma once
#include "nickel/graphics/enums.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class SamplerImpl;

class NICKEL_API Sampler {
public:
    struct Descriptor {
        Filter magFilter = Filter::Linear;
        Filter minFilter = Filter::Linear;
        SamplerMipmapMode mipmapMode = SamplerMipmapMode::Linear;
        SamplerAddressMode addressModeU = SamplerAddressMode::Repeat;
        SamplerAddressMode addressModeV = SamplerAddressMode::Repeat;
        SamplerAddressMode addressModeW = SamplerAddressMode::Repeat;
        float mipLodBias = 0.0;
        bool anisotropyEnable = false;
        float maxAnisotropy = 0.0;
        bool compareEnable = false;
        CompareOp compareOp = CompareOp::Always;
        float minLod = 0;
        float maxLod = 0;
        BorderColor borderColor = BorderColor::IntOpaqueWhite;
        bool unnormalizedCoordinates = false;
    };

    Sampler() = default;
    explicit Sampler(SamplerImpl*);
    Sampler(const Sampler&);
    Sampler(Sampler&&) noexcept;
    Sampler& operator=(const Sampler&) noexcept;
    Sampler& operator=(Sampler&&) noexcept;
    ~Sampler();

    const SamplerImpl& Impl() const noexcept;
    SamplerImpl& Impl() noexcept;

    operator bool() const noexcept;
    void Release();
    
private:
    SamplerImpl* m_impl{};
};

}  // namespace nickel::graphics