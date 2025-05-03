#pragma once
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;

class BindGroupPool {
public:
    BindGroupPool(DeviceImpl&, uint32_t descriptor_count_per_type);
    BindGroupPool(const BindGroupPool&) = delete;
    BindGroupPool(BindGroupPool&&) = delete;
    BindGroupPool& operator=(const BindGroupPool&) = delete;
    BindGroupPool& operator=(BindGroupPool&&) = delete;
    ~BindGroupPool();
    
    VkDescriptorPool m_pool;
    
private:
    DeviceImpl& m_device;
};

}