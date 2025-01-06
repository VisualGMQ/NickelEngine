#pragma once

namespace nickel::graphics {

class SamplerImpl;

class Sampler {
public:
    struct Descriptor {
        VkFilter magFilter;
        VkFilter minFilter;
        VkSamplerMipmapMode mipmapMode;
        VkSamplerAddressMode addressModeU;
        VkSamplerAddressMode addressModeV;
        VkSamplerAddressMode addressModeW;
        float mipLodBias;
        VkBool32 anisotropyEnable;
        float maxAnisotropy;
        VkBool32 compareEnable;
        VkCompareOp compareOp;
        float minLod;
        float maxLod;
        VkBorderColor borderColor;
        VkBool32 unnormalizedCoordinates;
    };
    
    explicit Sampler(SamplerImpl*);
    Sampler(const Sampler&);
    Sampler(Sampler&&) noexcept;
    Sampler& operator=(const Sampler&) noexcept;
    Sampler& operator=(Sampler&&) noexcept;
    ~Sampler();

    const SamplerImpl& Impl() const noexcept;
    SamplerImpl& Impl() noexcept;

private:
    SamplerImpl* m_impl;
};

}  // namespace nickel::graphics