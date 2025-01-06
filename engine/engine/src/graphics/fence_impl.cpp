﻿#include "nickel/graphics/internal/fence_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

FenceImpl::FenceImpl(const DeviceImpl& device, bool signaled)
    : m_device{device} {
    VkFenceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ci.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
    VK_CALL(vkCreateFence(device.m_device, &ci, nullptr, &m_fence));
}

FenceImpl::~FenceImpl() {
    vkDestroyFence(m_device.m_device, m_fence, nullptr);
}

}  // namespace nickel::graphics