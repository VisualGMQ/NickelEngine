#include "nickel/graphics/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/internal/bind_group_impl.hpp"
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

        NICKEL_CANT_REACH();
        return {};
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

        NICKEL_CANT_REACH();
        return {};
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
    : m_device{dev} {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto&& [slot, entry] : desc.entries) {
        bindings.emplace_back(getBinding(slot, entry));
    }

    VkDescriptorSetLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ci.bindingCount = bindings.size();
    ci.pBindings = bindings.data();

    VK_CALL(vkCreateDescriptorSetLayout(dev.m_device, &ci, nullptr, &m_layout));

    uint32_t count =
        dev.GetSwapchainImageInfo().m_image_count * MaxDrawCallPerCmdBuf;
    createPool(count, desc);
    allocSets(count);
}

BindGroup BindGroupLayoutImpl::RequireBindGroup(
    const BindGroup::Descriptor& desc) {
    size_t idx;
    VkDescriptorSet set = RequireSet(idx);
    BindGroup bind_group{
        m_bind_group_allocator.Allocate(m_device, idx, set, *this, desc)};
    bind_group.Impl().WriteDescriptors();
    return bind_group;
}

VkDescriptorSet BindGroupLayoutImpl::RequireSet(size_t& out_idx) {
    if (m_unused_group_indices.empty()) {
        LOGW("run out of descriptor set!");
        return VK_NULL_HANDLE;
    }

    out_idx = m_unused_group_indices.back();
    m_unused_group_indices.pop_back();
    return m_groups[out_idx];
}

void BindGroupLayoutImpl::RecycleSetList(size_t index) {
    m_unused_group_indices.push_back(index);
}

VkDescriptorSetLayoutBinding BindGroupLayoutImpl::getBinding(
    uint32_t slot, const BindGroupLayout::Entry& entry) {
    VkDescriptorSetLayoutBinding binding{};

    binding.binding = slot;
    binding.descriptorCount = entry.arraySize;
    binding.stageFlags = ShaderStage2Vk(entry.shader_stage);
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

    VK_CALL(vkCreateDescriptorPool(m_device.m_device, &info, nullptr, &m_pool));
}

void BindGroupLayoutImpl::allocSets(uint32_t count) {
    std::vector<VkDescriptorSetLayout> layouts{count, m_layout};
    VkDescriptorSetAllocateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    info.descriptorPool = m_pool;
    info.pSetLayouts = layouts.data();
    info.descriptorSetCount = count;

    m_groups.resize(count);
    VK_CALL(
        vkAllocateDescriptorSets(m_device.m_device, &info, m_groups.data()));
    m_unused_group_indices.reserve(count);
    for (size_t i = 0; i < count; i++) {
        m_unused_group_indices.push_back(i) ;
    }
}

BindGroupLayoutImpl::~BindGroupLayoutImpl() {
    m_bind_group_allocator.FreeAll();
    VK_CALL(vkResetDescriptorPool(m_device.m_device, m_pool, 0));
    vkDestroyDescriptorPool(m_device.m_device, m_pool, nullptr);
    vkDestroyDescriptorSetLayout(m_device.m_device, m_layout, nullptr);
}

void BindGroupLayoutImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_pending_delete_bind_group_layouts.push_back(this);
    }
}

}  // namespace nickel::graphics
