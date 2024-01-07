#include "rhi/vulkan/shader.hpp"
#include "rhi/vulkan/device.hpp"

namespace nickel::rhi::vulkan {

ShaderModule::ShaderModule(Device* device,
                           const std::filesystem::path& filename, Type type)
    : rhi::ShaderModule{type}, device_(device) {
    std::ifstream file(filename);

    if (!file) {
        LOGW(nickel::log_tag::Vulkan, "load shader from ", filename, " failed");
    } else {
        std::vector<char> data(std::istreambuf_iterator<char>(file),
                               (std::istreambuf_iterator<char>()));

        vk::ShaderModuleCreateInfo createInfo;
        createInfo.pCode = (uint32_t*)data.data();
        createInfo.codeSize = data.size();
        module_ = device_->Raw().createShaderModule(createInfo);
        if (!module_) {
            LOGW(nickel::log_tag::Vulkan, "create shader from", filename, " failed!");
        }
    }
}

ShaderModule::~ShaderModule() {
    if (device_ && module_) {
        device_->Raw().destroyShaderModule(module_);
    }
}

vk::PipelineShaderStageCreateInfo ShaderModule::GetShaderStageCreateInfo() {
    vk::PipelineShaderStageCreateInfo createInfo;
    createInfo.setModule(module_).setStage(ShaderType2Vulkan(type_));
    return createInfo;
}

vk::ShaderStageFlagBits ShaderType2Vulkan(ShaderModule::Type type) {
    switch (type) {
        case ShaderModule::Type::Vertex:
            return vk::ShaderStageFlagBits::eVertex;
        case ShaderModule::Type::Fragment:
            return vk::ShaderStageFlagBits::eFragment;
        case ShaderModule::Type::Compute:
            return vk::ShaderStageFlagBits::eCompute;
        case ShaderModule::Type::Unknown:
            Assert(false, "can't convert unknown to vulkan shader stage");
            return {};
    }
}

}  // namespace nickel::rhi::vulkan