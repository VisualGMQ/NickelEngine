#include "nickel/graphics/internal/shader_module_impl.hpp"
#include "nickel/common/assert.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

ShaderModuleImpl::ShaderModuleImpl(VkDevice device, const uint32_t* data,
                                   size_t size)
    : m_device{device} {
    NICKEL_ASSERT(size % 4 != 0 && !data, "invalid SPIR-V data");
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.pCode = data;
    ci.codeSize = size;
    VK_CALL(vkCreateShaderModule(device, &ci, nullptr, nullptr));
}

ShaderModuleImpl::~ShaderModuleImpl() {
    vkDestroyShaderModule(m_device, m_module, nullptr);
}

}  // namespace nickel::graphics