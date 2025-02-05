#pragma once
#include "nickel/internal/pch.hpp"
#include "nickel/common/memory/refcountable.hpp"

namespace nickel::graphics {

class DeviceImpl;

class ShaderModuleImpl: public RefCountable  {
public:
    ShaderModuleImpl(DeviceImpl&, const uint32_t* data, size_t size);
    ShaderModuleImpl(const ShaderModuleImpl&) = delete;
    ShaderModuleImpl(ShaderModuleImpl&&) = delete;
    ShaderModuleImpl& operator=(const ShaderModuleImpl&) = delete;
    ShaderModuleImpl& operator=(ShaderModuleImpl&&) = delete;

    ~ShaderModuleImpl();

    void DecRefcount() override;

    VkShaderModule m_module = VK_NULL_HANDLE;

private:
    DeviceImpl& m_device;
};


}