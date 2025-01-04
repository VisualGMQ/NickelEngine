#include "nickel/graphics/internal/cmd_pool_impl.hpp"

#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

CommandPoolImpl::CommandPoolImpl(DeviceImpl& device,
                                 VkCommandPoolCreateFlags flag) {
    VkCommandPoolCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ci.flags = flag;
    ci.queueFamilyIndex = device.m_queue_indices.m_graphics_index.value();

    m_can_reset_single_cmd =
        flag & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL(vkCreateCommandPool(device.m_device, &ci, nullptr, &m_pool));
}

CommandPoolImpl::~CommandPoolImpl() {
    VK_CALL(vkResetCommandPool(m_device, m_pool, 0));
    vkDestroyCommandPool(m_device, m_pool, nullptr);
}

bool CommandPoolImpl::CanResetSingleCmd() const noexcept {
    return m_can_reset_single_cmd;
}

}  // namespace nickel::graphics