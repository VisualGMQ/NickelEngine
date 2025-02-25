#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/graphics/lowlevel/enums.hpp"

namespace nickel::graphics {

class SamplerImpl;

class NICKEL_API Sampler: public ImplWrapper<SamplerImpl> {
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

    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics