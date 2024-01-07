#include "rhi/vulkan/descriptor.hpp"
#include "rhi/vulkan/device.hpp"

namespace nickel::rhi::vulkan {

DescriptorSetLayout::DescriptorSetLayout(
    Device* device, const std::vector<DescriptorSetLayoutBinding>& bindings)
    : device_(device) {
    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
    for (auto& binding : bindings) {
        layoutBindings.emplace_back(DescriptorSetLayoutBinding2Vk(binding));
    }

    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo({}, layoutBindings);

    layout_ = device->Raw().createDescriptorSetLayout(layoutCreateInfo);
    if (!layout_) {
        LOGW(log_tag::Vulkan, "create descriptor set layout failed");
    }
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (device_ && layout_) {
        device_->Raw().destroyDescriptorSetLayout(layout_);
    }
}

}  // namespace nickel::rhi::vulkan