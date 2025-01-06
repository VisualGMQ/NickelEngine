#include "nickel/graphics/internal/sampler_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

SamplerImpl::SamplerImpl(DeviceImpl& dev, const Sampler::Descriptor& desc)
    : m_dev{dev.m_device} {
    VkSamplerCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ci.magFilter = desc.magFilter;
    ci.minFilter = desc.minFilter;
    ci.mipmapMode = desc.mipmapMode;
    ci.addressModeU = desc.addressModeU;
    ci.addressModeV = desc.addressModeV;
    ci.addressModeW = desc.addressModeW;
    ci.mipLodBias = desc.mipLodBias;
    ci.anisotropyEnable = desc.anisotropyEnable;
    ci.maxAnisotropy = desc.maxAnisotropy;
    ci.compareEnable = desc.compareEnable;
    ci.compareOp = desc.compareOp;
    ci.minLod = desc.minLod;
    ci.maxLod = desc.maxLod;
    ci.borderColor = desc.borderColor;
    ci.unnormalizedCoordinates = desc.unnormalizedCoordinates;
    
    VK_CALL(vkCreateSampler(m_dev, &ci, nullptr, &m_sampler));
}

SamplerImpl::~SamplerImpl() {
    vkDestroySampler(m_dev, m_sampler, nullptr);
}

}  // namespace nickel::graphics