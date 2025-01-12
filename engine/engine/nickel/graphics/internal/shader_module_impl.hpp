#pragma once
#include "nickel/internal/pch.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class ShaderModuleImpl: public RefCountable  {
public:
    ShaderModuleImpl(DeviceImpl&, const uint32_t* data, size_t size);
    ~ShaderModuleImpl();

    void PendingDelete();

    VkShaderModule m_module;

private:
    DeviceImpl& m_device;
};


}