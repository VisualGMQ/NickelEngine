#include "vulkan/shader.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

ShaderModule::ShaderModule(Device* device, vk::ShaderStageFlagBits type,
                           const std::vector<char>& content,
                           const std::string& entry)
    : type_{type}, device_(device), entry_{entry} {
    Assert(content.size() % 4 == 0 && !content.empty(), "invalid SPIR-V data");
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.pCode = (uint32_t*)content.data();
    createInfo.codeSize = content.size();
    VK_CALL(module_, device_->GetDevice().createShaderModule(createInfo));
}

ShaderModule::~ShaderModule() {
    if (device_ && module_) {
        device_->GetDevice().destroyShaderModule(module_);
    }
}

vk::PipelineShaderStageCreateInfo ShaderModule::GetShaderStageCreateInfo() {
    vk::PipelineShaderStageCreateInfo createInfo;
    createInfo.setModule(module_).setStage(GetType()).setPName(entry_.c_str());
    return createInfo;
}

}  // namespace nickel::vulkan