#pragma once
#include "nickel/graphics/enums.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class SamplerImpl;

class NICKEL_API Sampler {
public:
    struct Descriptor {
        Filter magFilter;
        Filter minFilter;
        SamplerMipmapMode mipmapMode;
        SamplerAddressMode addressModeU;
        SamplerAddressMode addressModeV;
        SamplerAddressMode addressModeW;
        float mipLodBias;
        bool anisotropyEnable;
        float maxAnisotropy;
        bool compareEnable;
        CompareOp compareOp;
        float minLod;
        float maxLod;
        BorderColor borderColor;
        bool unnormalizedCoordinates;
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