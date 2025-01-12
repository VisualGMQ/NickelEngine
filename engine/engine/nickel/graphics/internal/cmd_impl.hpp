#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/graphics/internal/image_impl.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class CommandPoolImpl;

class CommandImpl {
public:
    enum class Flag : uint32_t {
        Unknown = 0,
        Render = 0x01,
        Transfer = 0x02,
    };

    explicit CommandImpl(DeviceImpl& device, CommandPoolImpl& pool,
                         VkCommandBuffer cmd);
    ~CommandImpl();

    VkCommandBuffer m_cmd;
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