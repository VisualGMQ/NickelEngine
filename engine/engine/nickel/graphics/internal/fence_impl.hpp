#pragma once
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;

class FenceImpl : public RefCountable {
public:
    FenceImpl(DeviceImpl&, bool signaled);
    ~FenceImpl();

    VkFence m_fence;

    void PendingDelete();

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics