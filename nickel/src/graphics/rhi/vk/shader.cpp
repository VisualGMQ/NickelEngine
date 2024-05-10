#include "graphics/rhi/vk/shader.hpp"
#include "common/assert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/util.hpp"


namespace nickel::rhi::vulkan {

ShaderModuleImpl::ShaderModuleImpl(vk::Device dev,
                                   const std::vector<char>& code)
    : dev_{dev} {
    Assert(code.size() % 4 == 0 && !code.empty(), "invalid SPIR-V data");
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.pCode = (uint32_t*)code.data();
    createInfo.codeSize = code.size();
    VK_CALL(module, dev.createShaderModule(createInfo));
}

ShaderModuleImpl::~ShaderModuleImpl() {
    dev_.destroyShaderModule(module);
}

}  // namespace nickel::rhi::vulkan