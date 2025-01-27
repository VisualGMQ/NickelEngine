#pragma once
#include "nickel/common/memory/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class SemaphoreImpl : public RefCountable {
public:
    SemaphoreImpl(DeviceImpl&);
    SemaphoreImpl(const SemaphoreImpl&) = delete;
    SemaphoreImpl(SemaphoreImpl&&) = delete;
    SemaphoreImpl& operator=(const SemaphoreImpl&) = delete;
    SemaphoreImpl& operator=(SemaphoreImpl&&) = delete;

    ~SemaphoreImpl();

    void DecRefcount() override;

    VkSemaphore m_semaphore = VK_NULL_HANDLE;

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics