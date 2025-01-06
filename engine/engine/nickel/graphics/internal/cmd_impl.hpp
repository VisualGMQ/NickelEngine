#pragma once
#include "nickel/common/flags.hpp"
#include "nickel/graphics/internal/image_impl.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class CommandPoolImpl;

class CommandImpl : public RefCountable {
public:
    enum class Flag : uint32_t {
        Unknown = 0,
        Render = 0x01,
        Transfer = 0x02,
    };

    explicit CommandImpl(VkDevice device, CommandPoolImpl& pool,
                         VkCommandBuffer cmd);
    ~CommandImpl();

    VkCommandBuffer m_cmd;
    Flags<Flag> m_flags = Flag::Unknown;

    void AddLayoutTransition(ImageImpl*, VkImageLayout, size_t idx);
    void ApplyLayoutTransitions();

private:
    VkDevice m_device;
    CommandPoolImpl& m_pool;
    std::unordered_map<ImageImpl*, std::unordered_map<size_t, VkImageLayout>>
        m_layout_transitions;
};

}  // namespace nickel::graphics