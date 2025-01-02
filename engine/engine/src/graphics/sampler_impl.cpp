#include "nickel/graphics/internal/sampler_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

SamplerImpl::SamplerImpl(DeviceImpl& dev, const VkSamplerCreateInfo& ci)
    : m_dev{dev.m_device} {
    VK_CALL(vkCreateSampler(m_dev, &ci, nullptr, &m_sampler));
}

SamplerImpl::~SamplerImpl() {
    vkDestroySampler(m_dev, m_sampler, nullptr);
}

}  // namespace nickel::graphics