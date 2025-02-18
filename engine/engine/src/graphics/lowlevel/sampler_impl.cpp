#include "nickel/graphics/lowlevel/internal/sampler_impl.hpp"

#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

SamplerImpl::SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor& desc)
    : m_dev{dev} {
    VkSamplerCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ci.magFilter = Filter2Vk(desc.m_mag_filter);
    ci.minFilter = Filter2Vk(desc.m_min_filter);
    ci.mipmapMode = SamplerMipmapMode2Vk(desc.m_mipmap_mode);
    ci.addressModeU = SamplerAddressMode2Vk(desc.m_address_mode_u);
    ci.addressModeV = SamplerAddressMode2Vk(desc.m_address_mode_v);
    ci.addressModeW = SamplerAddressMode2Vk(desc.m_address_mode_w);
    ci.mipLodBias = desc.m_mip_lod_bias;
    ci.anisotropyEnable = desc.m_anisotropy_enable;
    ci.maxAnisotropy = desc.m_max_anisotropy;
    ci.compareEnable = desc.m_compare_enable;
    ci.compareOp = CompareOp2Vk(desc.m_compare_op);
    ci.minLod = desc.m_min_lod;
    ci.maxLod = desc.m_max_lod;
    ci.borderColor = BorderColor2Vk(desc.m_border_color);
    ci.unnormalizedCoordinates = desc.m_unnormalized_coordinates;
    
    VK_CALL(vkCreateSampler(m_dev.m_device, &ci, nullptr, &m_sampler));
}

SamplerImpl::~SamplerImpl() {
    vkDestroySampler(m_dev.m_device, m_sampler, nullptr);
}

void SamplerImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_dev.m_sampler_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::graphics