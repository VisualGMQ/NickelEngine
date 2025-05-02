#include "nickel/graphics/lowlevel/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_impl.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_pool.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/enum_convert.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

constexpr uint32_t MaxDrawCallPerCmdBuf = 128;

struct getDescriptorTypeHelper {
    VkDescriptorType operator()(const BindGroup::BufferBinding& binding) const {
        switch (binding.m_type) {
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
        switch (binding.m_type) {
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
    return std::visit(getDescriptorTypeHelper{}, entry.m_entry);
}

BindGroupLayoutImpl::BindGroupLayoutImpl(
    DeviceImpl& dev, const BindGroupLayout::Descriptor& desc,
    BindGroupPool& pool, uint32_t descriptor_set_count)
    : m_device{dev} {
    m_layout = createLayout(dev, desc);
    createSets(dev, pool.m_pool, descriptor_set_count);
}

VkDescriptorSetLayout BindGroupLayoutImpl::createLayout(
    DeviceImpl& dev, const BindGroupLayout::Descriptor& desc) {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (auto&& [slot, entry] : desc.m_entries) {
        bindings.emplace_back(getBinding(slot, entry));
    }

    VkDescriptorSetLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ci.bindingCount = bindings.size();
    ci.pBindings = bindings.data();

    VkDescriptorSetLayout layout = VK_NULL_HANDLE;

    VK_CALL(vkCreateDescriptorSetLayout(dev.m_device, &ci, nullptr, &layout));

    return layout;
}

void BindGroupLayoutImpl::createSets(DeviceImpl& dev, VkDescriptorPool pool,
                                     uint32_t descriptor_set_count) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    std::vector layouts{descriptor_set_count, m_layout};
    alloc_info.pSetLayouts = layouts.data();
    alloc_info.descriptorSetCount = descriptor_set_count;
    m_descriptor_sets.resize(descriptor_set_count);

    VK_CALL(vkAllocateDescriptorSets(dev.m_device, &alloc_info,
                                     m_descriptor_sets.data()));

    m_unused_descriptor_set.reserve(descriptor_set_count);
    for (int i = 0; i < descriptor_set_count; i++) {
        m_unused_descriptor_set.push_back(i);
    }
}

BindGroup BindGroupLayoutImpl::RequireBindGroup(
    const BindGroup::Descriptor& desc) {
    if (m_unused_descriptor_set.empty()) {
        LOGE("use too many descriptor set in one frame! descriptor set max "
             "count is ",
             m_descriptor_sets.size());
        return {};
    }

    uint32_t unused_set_idx = m_unused_descriptor_set.back();
    m_unused_descriptor_set.pop_back();
    return BindGroup{m_bind_group_allocator.Allocate(
        m_device, *this, desc, m_descriptor_sets[unused_set_idx],
        unused_set_idx)};
}

VkDescriptorSetLayoutBinding BindGroupLayoutImpl::getBinding(
    uint32_t slot, const BindGroupLayout::Entry& entry) {
    VkDescriptorSetLayoutBinding binding{};

    binding.binding = slot;
    binding.descriptorCount = entry.m_array_size;
    binding.stageFlags = ShaderStage2Vk(entry.m_shader_stage);
    binding.descriptorType = BindGroupEntryType2Vk(entry.m_type);
    return binding;
}

BindGroupLayoutImpl::~BindGroupLayoutImpl() {
    m_bind_group_allocator.FreeAll();
    vkDestroyDescriptorSetLayout(m_device.m_device, m_layout, nullptr);
}

void BindGroupLayoutImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_device.m_bind_group_layout_allocator.MarkAsGarbage(this);
    }
}

void BindGroupLayoutImpl::GC() {
    m_bind_group_allocator.GC();
}

void BindGroupLayoutImpl::RecycleBindGroup(const BindGroupImpl& bind_group) {
    m_unused_descriptor_set.push_back(bind_group.GetID());
}

}  // namespace nickel::graphics
