#pragma once
#include "nickel/internal/pch.hpp"
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class ShaderModuleImpl: public RefCountable  {
public:
    ShaderModuleImpl(VkDevice, const std::vector<char>& code);
    ~ShaderModuleImpl();

    VkShaderModule m_module;

private:
    VkDevice m_device;
};


}