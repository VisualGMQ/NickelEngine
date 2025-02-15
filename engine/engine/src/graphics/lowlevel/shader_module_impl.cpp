#include "nickel/graphics/lowlevel/internal/shader_module_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"
#include "nickel/common/assert.hpp"

namespace nickel::graphics {

ShaderModuleImpl::ShaderModuleImpl(DeviceImpl& device, const uint32_t* data,
                                   size_t size)
    : m_device{device} {
    NICKEL_ASSERT(size % 4 == 0 && data, "invalid SPIR-V data");
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.pCode = data;
    ci.codeSize = size;
    VK_CALL(vkCreateShaderModule(device.m_device, &ci, nullptr, &m_module));
}

ShaderModuleImpl::~ShaderModuleImpl() {
    vkDestroyShaderModule(m_device.m_device, m_module, nullptr);
}

void ShaderModuleImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_shader_module_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::graphics