#pragma once
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;

class FenceImpl : public RefCountable {
public:
    FenceImpl(DeviceImpl&, bool signaled);
    FenceImpl(const FenceImpl&) = delete;
    FenceImpl(FenceImpl&&) = delete;
    FenceImpl& operator=(const FenceImpl&) = delete;
    FenceImpl& operator=(FenceImpl&&) = delete;

    ~FenceImpl();

    VkFence m_fence = VK_NULL_HANDLE;

    void DecRefcount() override;

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics