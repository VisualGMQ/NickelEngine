#pragma once
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class CommandPoolImpl : public RefCountable {
public:
    CommandPoolImpl(DeviceImpl& device, VkCommandPoolCreateFlags flags);
    ~CommandPoolImpl();
    bool CanResetSingleCmd() const noexcept;

    VkCommandPool m_pool;

private:
    VkDevice m_device;
    bool m_can_reset_single_cmd = false;;
};

}  // namespace nickel::graphics