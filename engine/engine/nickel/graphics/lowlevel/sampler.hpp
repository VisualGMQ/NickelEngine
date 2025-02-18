#pragma once
#include "nickel/graphics/lowlevel/enums.hpp"
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class SamplerImpl;

class NICKEL_API Sampler {
public:
    struct Descriptor {
        Filter m_mag_filter = Filter::Linear;
        Filter m_min_filter = Filter::Linear;
        SamplerMipmapMode m_mipmap_mode = SamplerMipmapMode::Linear;
        SamplerAddressMode m_address_mode_u = SamplerAddressMode::Repeat;
        SamplerAddressMode m_address_mode_v = SamplerAddressMode::Repeat;
        SamplerAddressMode m_address_mode_w = SamplerAddressMode::Repeat;
        float m_mip_lod_bias = 0.0;
        bool m_anisotropy_enable = false;
        float m_max_anisotropy = 0.0;
        bool m_compare_enable = false;
        CompareOp m_compare_op = CompareOp::Always;
        float m_min_lod = 0;
        float m_max_lod = 0;
        BorderColor m_border_color = BorderColor::IntOpaqueWhite;
        bool m_unnormalized_coordinates = false;
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