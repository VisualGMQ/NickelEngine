#include "nickel/graphics/lowlevel/internal/bind_group_pool.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

BindGroupPool::BindGroupPool(DeviceImpl& dev,
                                     uint32_t descriptor_count_per_type)
    : m_device{dev} {
    VkDescriptorPoolCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    std::vector<VkDescriptorPoolSize> pool_sizes;

    std::array types = {
        VK_DESCRIPTOR_TYPE_SAMPLER,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
        VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
        VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    };

    uint32_t max_set_num = 0;
    for (auto&& type : types) {
        VkDescriptorPoolSize size;
        size.descriptorCount = descriptor_count_per_type;
        size.type = type;
        max_set_num += size.descriptorCount;
        pool_sizes.emplace_back(size);
    }
    ci.maxSets = max_set_num;
    ci.poolSizeCount = pool_sizes.size();
    ci.pPoolSizes = pool_sizes.data();

    VK_CALL(vkCreateDescriptorPool(dev.m_device, &ci, nullptr, &m_pool));
}

BindGroupPool::~BindGroupPool() {
    vkDestroyDescriptorPool(m_device.m_device, m_pool, nullptr);
}

}  // namespace nickel::graphics