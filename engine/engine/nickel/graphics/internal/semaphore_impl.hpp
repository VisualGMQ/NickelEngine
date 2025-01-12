#pragma once
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class SemaphoreImpl : public RefCountable {
public:
    SemaphoreImpl(DeviceImpl&);
    ~SemaphoreImpl();
    void PendingDelete();

    VkSemaphore m_semaphore;

private:
    DeviceImpl& m_device;
};

}  // namespace nickel::graphics