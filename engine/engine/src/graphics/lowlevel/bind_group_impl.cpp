#include "nickel/graphics/lowlevel/internal/bind_group_impl.hpp"

#include "nickel/graphics/lowlevel/internal/buffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"
#include "nickel/graphics/lowlevel/internal/sampler_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

BindGroupImpl::BindGroupImpl(DeviceImpl& dev, BindGroupLayoutImpl& layout,
                             const BindGroup::Descriptor& desc,
                             VkDescriptorSet descriptor_set, uint32_t id)
    : m_layout{&layout},
      m_device{dev},
      m_desc{desc},
      m_descriptor_set{descriptor_set},
      m_id{id} {
    writeDescriptors(desc);
}

void BindGroupImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_layout.GetImpl()->RecycleBindGroup(*this);
        m_layout.GetImpl()->m_bind_group_allocator.MarkAsGarbage(this);
    }
}

VkDescriptorType cvtBufferType2DescriptorType(
    BindGroup::BufferBinding::Type type) {
    switch (type) {
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

VkDescriptorType cvtImageType2DescriptorType(
    BindGroup::ImageBinding::Type type) {
    switch (type) {
        case BindGroup::ImageBinding::Type::Image:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case BindGroup::ImageBinding::Type::StorageImage:
            return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    }

    NICKEL_CANT_REACH();
    return {};
}

struct WriteDescriptorHelper final {
    explicit WriteDescriptorHelper(DeviceImpl& device, uint32_t slot,
                                   VkDescriptorSet descriptor_set)
        : m_device{device}, m_slot{slot}, m_descriptor_set{descriptor_set} {}

    void operator()(const BindGroup::BufferBinding& binding) const {
        if (binding.m_buffer.GetImpl()->Size() == 0) {
            LOGW("m_buffer m_binding m_size == 0");
            return;
        }

        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorBufferInfo buffer_info{};
        if (binding.m_type != BindGroup::BufferBinding::Type::DynamicUniform &&
            binding.m_type != BindGroup::BufferBinding::Type::DynamicStorage) {
            buffer_info.offset = binding.m_offset ? binding.m_offset.value() : 0;
        }

        auto buffer_impl = binding.m_buffer.GetImpl();
        buffer_info.buffer = buffer_impl->m_buffer;
        buffer_info.range =
            binding.m_size ? binding.m_size.value() : buffer_impl->Size();

        write_info.descriptorCount = 1;
        write_info.descriptorType =
            cvtBufferType2DescriptorType(binding.m_type);
        write_info.dstArrayElement = 0;
        write_info.pBufferInfo = &buffer_info;
        write_info.dstBinding = m_slot;
        write_info.dstSet = m_descriptor_set;

        vkUpdateDescriptorSets(m_device.m_device, 1, &write_info, 0, nullptr);
    }

    void operator()(const BindGroup::SamplerBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.sampler = binding.m_sampler.GetImpl()->m_sampler;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &image_info;
        write_info.dstArrayElement = 0;
        write_info.dstBinding = m_slot;
        write_info.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        write_info.dstSet = m_descriptor_set;

        vkUpdateDescriptorSets(m_device.m_device, 1, &write_info, 0, nullptr);
    }

    void operator()(const BindGroup::ImageBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.imageView = binding.m_view.GetImpl()->m_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &image_info;
        write_info.dstArrayElement = 0;
        write_info.dstSet = m_descriptor_set;
        write_info.dstBinding = m_slot;
        write_info.descriptorType = cvtImageType2DescriptorType(binding.m_type);

        vkUpdateDescriptorSets(m_device.m_device, 1, &write_info, 0, nullptr);
    }

    void operator()(const BindGroup::CombinedSamplerBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.imageView = binding.m_view.GetImpl()->m_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.sampler = binding.m_sampler.GetImpl()->m_sampler;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &image_info;
        write_info.dstArrayElement = 0;
        write_info.dstSet = m_descriptor_set;
        write_info.dstBinding = m_slot;
        write_info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        vkUpdateDescriptorSets(m_device.m_device, 1, &write_info, 0, nullptr);
    }

private:
    DeviceImpl& m_device;
    VkDescriptorSet m_descriptor_set;
    uint32_t m_slot{};
};

void BindGroupImpl::writeDescriptors(const BindGroup::Descriptor& desc) const {
    for (auto&& [slot, entry] : desc.m_entries) {
        WriteDescriptorHelper helper{m_device, slot, m_descriptor_set};
        std::visit(helper, entry.m_binding.m_entry);
    }
}

const BindGroup::Descriptor& BindGroupImpl::GetDescriptor() const {
    return m_desc;
}

uint32_t BindGroupImpl::GetID() const {
    return m_id;
}

}  // namespace nickel::graphics