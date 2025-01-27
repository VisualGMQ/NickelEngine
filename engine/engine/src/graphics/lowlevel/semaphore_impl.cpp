#include "nickel/graphics/lowlevel/internal/semaphore_impl.hpp"

#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

SemaphoreImpl::SemaphoreImpl(DeviceImpl& device) : m_device{device} {
    VkSemaphoreCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CALL(vkCreateSemaphore(device.m_device, &ci, nullptr, &m_semaphore));
}

SemaphoreImpl::~SemaphoreImpl() {
    vkDestroySemaphore(m_device.m_device, m_semaphore, nullptr);
}

void SemaphoreImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_pending_delete_semaphores.push_back(this);
    }
}

}  // namespace nickel::graphics