#include "nickel/graphics/internal/sampler_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

SamplerImpl::SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor& desc)
    : m_dev{dev} {
    VkSamplerCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ci.magFilter = Filter2Vk(desc.magFilter);
    ci.minFilter = Filter2Vk(desc.minFilter);
    ci.mipmapMode = SamplerMipmapMode2Vk(desc.mipmapMode);
    ci.addressModeU = SamplerAddressMode2Vk(desc.addressModeU);
    ci.addressModeV = SamplerAddressMode2Vk(desc.addressModeV);
    ci.addressModeW = SamplerAddressMode2Vk(desc.addressModeW);
    ci.mipLodBias = desc.mipLodBias;
    ci.anisotropyEnable = desc.anisotropyEnable;
    ci.maxAnisotropy = desc.maxAnisotropy;
    ci.compareEnable = desc.compareEnable;
    ci.compareOp = CompareOp2Vk(desc.compareOp);
    ci.minLod = desc.minLod;
    ci.maxLod = desc.maxLod;
    ci.borderColor = BorderColor2Vk(desc.borderColor);
    ci.unnormalizedCoordinates = desc.unnormalizedCoordinates;
    
    VK_CALL(vkCreateSampler(m_dev.m_device, &ci, nullptr, &m_sampler));
}

SamplerImpl::~SamplerImpl() {
    vkDestroySampler(m_dev.m_device, m_sampler, nullptr);
}

void SamplerImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_dev.m_pending_delete_samplers.push_back(this);
    }
}

}  // namespace nickel::graphics