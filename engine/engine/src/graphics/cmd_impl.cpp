#include "nickel/graphics/internal/cmd_impl.hpp"

#include "nickel/graphics/internal/cmd_pool_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

CommandImpl::CommandImpl(VkDevice device, CommandPoolImpl& pool,
                         VkCommandBuffer cmd)
    : m_cmd{cmd}, m_device{device}, m_pool{pool} {}

CommandImpl::~CommandImpl() {
    if (m_pool.CanResetSingleCmd()) {
        VK_CALL(vkResetCommandBuffer(m_cmd, 0));
    }
}

}  // namespace nickel::graphics