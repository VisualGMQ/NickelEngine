#include "graphics/rhi/vk/shader.hpp"
#include "common/assert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/util.hpp"


namespace nickel::rhi::vulkan {

ShaderModuleImpl::ShaderModuleImpl(vk::Device dev,
                                   const std::vector<char>& code) {
    Assert(code.size() % 4 == 0 && !code.empty(), "invalid SPIR-V data");
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.pCode = (uint32_t*)code.data();
    createInfo.codeSize = code.size();
    VK_CALL(module, dev.createShaderModule(createInfo));
}

void ShaderModuleImpl::Destroy(rhi::DeviceImpl& dev) {
    static_cast<DeviceImpl&>(dev).device.destroyShaderModule(module);
}

}  // namespace nickel::rhi::vulkan