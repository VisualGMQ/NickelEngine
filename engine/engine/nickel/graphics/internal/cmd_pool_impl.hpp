#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/cmd.hpp"
#include "nickel/graphics/cmd_encoder.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class CommandPoolImpl : public RefCountable {
public:
    CommandPoolImpl(DeviceImpl& device, VkCommandPoolCreateFlags flags);
    ~CommandPoolImpl();
    bool CanResetSingleCmd() const noexcept;
    CommandEncoder CreateCommandEncoder();

    void Reset();

    VkCommandPool m_pool;
    BlockMemoryAllocator<CommandImpl> m_cmd_allocator;
    std::vector<CommandImpl*> m_pending_delete_cmds;

private:
    DeviceImpl& m_device;
    bool m_can_reset_single_cmd = false;
    ;
};

}  // namespace nickel::graphics