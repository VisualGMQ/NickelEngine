#include "nickel/graphics/lowlevel/internal/cmd_pool_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"
#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"

namespace nickel::graphics {

CommandPoolImpl::CommandPoolImpl(DeviceImpl& device,
                                 VkCommandPoolCreateFlags flag)
    : m_device{device} {
    VkCommandPoolCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ci.flags = flag;
    ci.queueFamilyIndex = device.m_queue_indices.m_graphics_index.value();

    m_can_reset_single_cmd =
        flag & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL(vkCreateCommandPool(device.m_device, &ci, nullptr, &m_pool));
}

CommandPoolImpl::~CommandPoolImpl() {
    VK_CALL(vkResetCommandPool(m_device.m_device, m_pool, 0));
    vkDestroyCommandPool(m_device.m_device, m_pool, nullptr);
}

bool CommandPoolImpl::CanResetSingleCmd() const noexcept {
    return m_can_reset_single_cmd;
}

void CommandPoolImpl::Reset() {
    VK_CALL(vkResetCommandPool(m_device.m_device, m_pool, 0));
    for (auto cmd : m_pending_delete_cmds) {
        vkFreeCommandBuffers(m_device.m_device, m_pool, 1, &cmd->m_cmd);
        m_cmd_allocator.Deallocate(cmd);
    }
    m_pending_delete_cmds.clear();
}

CommandEncoder CommandPoolImpl::CreateCommandEncoder() {
    VkCommandBuffer cmd;
    VkCommandBufferAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandPool = m_pool;
    info.commandBufferCount = 1;
    VK_CALL(vkAllocateCommandBuffers(m_device.m_device, &info, &cmd));
    return CommandEncoder{*m_cmd_allocator.Allocate(m_device, *this, cmd)};
}

}  // namespace nickel::graphics