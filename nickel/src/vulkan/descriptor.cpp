#include "vulkan/descriptor.hpp"
#include "vulkan/device.hpp"
#include "vulkan/util.hpp"

namespace nickel::vulkan {

DescriptorPool::DescriptorPool(Device* device,
                               const std::vector<vk::DescriptorPoolSize>& sizes,
                               uint32_t maxSets)
    : device_{device} {
    vk::DescriptorPoolCreateInfo info;
    info.setPoolSizes(sizes)
        .setMaxSets(maxSets);

    VK_CALL(pool_, device->GetDevice().createDescriptorPool(info));
}

DescriptorPool::~DescriptorPool() {
    if (device_ && pool_) {
        device_->GetDevice().destroyDescriptorPool(pool_);
    }
}

std::vector<vk::DescriptorSet> DescriptorPool::AllocSet(
    uint32_t count, const std::vector<vk::DescriptorSetLayout>& layouts) {
    vk::DescriptorSetAllocateInfo info;
    info.setDescriptorPool(pool_).setDescriptorSetCount(count).setSetLayouts(
        layouts);

    std::vector<vk::DescriptorSet> sets;
    VK_CALL(sets, device_->GetDevice().allocateDescriptorSets(info));
    return sets;
}

DescriptorSetLayout::DescriptorSetLayout(
    Device* device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
    : device_{device} {
    vk::DescriptorSetLayoutCreateInfo info;
    info.setBindings(bindings);
    VK_CALL(layout_, device->GetDevice().createDescriptorSetLayout(info));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (device_) {
        device_->GetDevice().destroyDescriptorSetLayout(layout_);
    }
}

}  // namespace nickel::vulkan