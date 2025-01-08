#include "nickel/graphics/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/enum_convert.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

constexpr uint32_t MaxDrawCallPerCmdBuf = 128;

struct getDescriptorTypeHelper {
    VkDescriptorType operator()(const BindGroup::BufferBinding& binding) const {
        switch (binding.type) {
            case BindGroup::BufferBinding::Type::Storage:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case BindGroup::BufferBinding::Type::Uniform:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case BindGroup::BufferBinding::Type::DynamicStorage:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
            case BindGroup::BufferBinding::Type::DynamicUniform:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        }
    }

    VkDescriptorType operator()(const BindGroup::SamplerBinding&) const {
        return VK_DESCRIPTOR_TYPE_SAMPLER;
    }

    VkDescriptorType operator()(const BindGroup::ImageBinding& binding) const {
        switch (binding.type) {
            case BindGroup::ImageBinding::Type::Image:
                return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            case BindGroup::ImageBinding::Type::StorageImage:
                return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        }
    }

    VkDescriptorType operator()(
        const BindGroup::CombinedSamplerBinding&) const {
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }
};

VkDescriptorType getDescriptorType(const BindGroup::BindingPoint& entry) {
    return std::visit(getDescriptorTypeHelper{}, entry.entry);
}

BindGroupLayoutImpl::BindGroupLayoutImpl(
    DeviceImpl& dev, const BindGroupLayout::Descriptor& desc)
    : m_device{dev.m_device}, m_group_elem_count{desc.entries.size()} {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto&& [slot, entry] : desc.entries) {
        bindings.emplace_back(getBinding(slot, entry));
    }

    VkDescriptorSetLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ci.bindingCount = bindings.size();
    ci.pBindings = bindings.data();

    VK_CALL(vkCreateDescriptorSetLayout(m_device, &ci, nullptr, &m_layout));

    uint32_t count =
        dev.GetSwapchainImageInfo().imagCount * MaxDrawCallPerCmdBuf;
    createPool(count, desc);
    allocSets(count);
}

const DescriptorSetLists* BindGroupLayoutImpl::RequireSetList() {
    if (m_unused_group_indices.empty()) {
        LOGW("run out of descriptor set!");
        return nullptr;
    }

    size_t idx = m_unused_group_indices.back();
    m_unused_group_indices.pop_back();
    return &m_groups[idx];
}

void BindGroupLayoutImpl::RecycleSetList(size_t index) {
    m_unused_group_indices.push_back(index);
}

VkDescriptorSetLayoutBinding BindGroupLayoutImpl::getBinding(
    uint32_t slot, const BindGroupLayout::Entry& entry) {
    VkDescriptorSetLayoutBinding binding{};

    binding.binding = slot;
    binding.descriptorCount = entry.arraySize;
    binding.stageFlags = static_cast<VkShaderStageFlagBits>(entry.shader_stage);
    binding.descriptorType = BindGroupEntryType2Vk(entry.type);
    return binding;
}

void BindGroupLayoutImpl::createPool(uint32_t count,
                                     const BindGroupLayout::Descriptor& desc) {
    VkDescriptorPoolCreateInfo info{};
    std::vector<VkDescriptorPoolSize> sizes;
    uint32_t maxCount = 0;
    for (auto&& [slot, entry] : desc.entries) {
        VkDescriptorPoolSize size;
        size.type = BindGroupEntryType2Vk(entry.type);
        size.descriptorCount = count;
        sizes.emplace_back(size);
        maxCount += count;
    }
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.maxSets = maxCount;
    info.poolSizeCount = sizes.size();
    info.pPoolSizes = sizes.data();

    VK_CALL(vkCreateDescriptorPool(m_device, &info, nullptr, &m_pool));
}

void BindGroupLayoutImpl::allocSets(uint32_t count) {
    std::vector<VkDescriptorSetLayout> layouts{count, m_layout};
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = m_pool;
    info.pSetLayouts = layouts.data();
    info.descriptorSetCount = count;

    std::vector<VkDescriptorSet> sets(count);
    VK_CALL(vkAllocateDescriptorSets(m_device, &info, sets.data()));

    for (int i = 0; i < MaxDrawCallPerCmdBuf; i++) {
        std::vector<VkDescriptorSet> set_group;
        for (int j = 0; j < m_group_elem_count; j++) {
            set_group.push_back(sets[i * m_group_elem_count + j]);
        }
        m_groups.emplace_back(std::move(set_group));
    }
}

BindGroupLayoutImpl::~BindGroupLayoutImpl() {
    VK_CALL(vkResetDescriptorPool(m_device, m_pool, 0));
    vkDestroyDescriptorPool(m_device, m_pool, nullptr);
    vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
}

}  // namespace nickel::graphics
