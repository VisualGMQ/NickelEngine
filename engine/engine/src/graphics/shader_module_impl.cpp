#include "nickel/graphics/internal/shader_module_impl.hpp"
#include "nickel/common/assert.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

ShaderModuleImpl::ShaderModuleImpl(VkDevice device,
                                   const std::vector<char>& code)
    : m_device{device} {
    NICKEL_ASSERT(code.size() % 4 == 0 && !code.empty(), "invalid SPIR-V data");
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.pCode = (uint32_t*)code.data();
    ci.codeSize = code.size();
    VK_CALL(vkCreateShaderModule(device, &ci, nullptr, nullptr));
}

ShaderModuleImpl::~ShaderModuleImpl() {
    vkDestroyShaderModule(m_device, m_module, nullptr);
}

}  // namespace nickel::graphics