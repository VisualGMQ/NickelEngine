#include "nickel/graphics/lowlevel/internal/cmd_impl.hpp"

#include "nickel/graphics/lowlevel/internal/cmd_pool_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

CommandEncoderImpl::CommandEncoderImpl(DeviceImpl& device, CommandPoolImpl& pool,
                         VkCommandBuffer cmd)
    : m_cmd{cmd}, m_device{device}, m_pool{pool} {}

CommandEncoderImpl::~CommandEncoderImpl() {
    if (m_pool.CanResetSingleCmd()) {
        VK_CALL(vkResetCommandBuffer(m_cmd, 0));
    }
}

void CommandEncoderImpl::AddLayoutTransition(ImageImpl* img, ImageLayout layout,
                                      size_t idx) {
    if (auto it = m_layout_transitions.find(img);
        it != m_layout_transitions.end()) {
        it->second[idx] = layout;
    } else {
        m_layout_transitions[img][idx] = layout;
    }
}

std::optional<VkImageLayout> CommandEncoderImpl::QueryImageLayout(
    ImageImpl* img, size_t idx) const {
    if (auto it = m_layout_transitions.find(img);
        it != m_layout_transitions.end()) {
        if (auto it2 = it->second.find(idx); it2 != it->second.end()) {
            return ImageLayout2Vk(it2->second);
        }
    }
    return {};
}

void CommandEncoderImpl::ApplyLayoutTransitions() {
    for (auto [image, transition] : m_layout_transitions) {
        for (auto [idx, layout] : transition) {
            image->m_layouts[idx] = layout;
        }
    }

    m_layout_transitions.clear();
}

void CommandEncoderImpl::PendingDelete() {
    m_pool.m_pending_delete_cmds.push_back(this);
}

}  // namespace nickel::graphics