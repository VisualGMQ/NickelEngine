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

void CommandImpl::AddLayoutTransition(ImageImpl* img, VkImageLayout layout,
                                      size_t idx) {
    if (auto it = m_layout_transitions.find(img);
        it != m_layout_transitions.end) {
        it->second[idx] = layout;
    } else {
        m_layout_transitions[img][idx] = layout;
    }
}

void CommandImpl::ApplyLayoutTransitions() {
    for (auto [image, transition] : m_layout_transitions) {
        for (auto [idx, layout] : transition) {
            image->m_layouts[idx] = layout;
        }
    }

    m_layout_transitions.clear();
}

}  // namespace nickel::graphics