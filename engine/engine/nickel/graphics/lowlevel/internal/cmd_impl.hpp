#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/graphics/lowlevel/internal/image_impl.hpp"

namespace nickel::graphics {

class CommandPoolImpl;

class CommandEncoderImpl {
public:
    enum class Flag : uint32_t {
        Unknown = 0,
        Render = 0x01,
        Transfer = 0x02,
    };

    explicit CommandEncoderImpl(DeviceImpl& device, CommandPoolImpl& pool,
                         VkCommandBuffer cmd);
    CommandEncoderImpl(const CommandEncoderImpl&) = delete;
    CommandEncoderImpl(CommandEncoderImpl&&) = delete;
    CommandEncoderImpl& operator=(const CommandEncoderImpl&) = delete;
    CommandEncoderImpl& operator=(CommandEncoderImpl&&) = delete;

    ~CommandEncoderImpl();

    VkCommandBuffer m_cmd = VK_NULL_HANDLE;
    Flags<Flag> m_flags = Flag::Unknown;

    void AddLayoutTransition(ImageImpl*, ImageLayout, size_t idx);
    void ApplyLayoutTransitions();

    void PendingDelete();

private:
    DeviceImpl& m_device;
    CommandPoolImpl& m_pool;
    std::unordered_map<ImageImpl*, std::unordered_map<size_t, ImageLayout>>
        m_layout_transitions;
};

}  // namespace nickel::graphics