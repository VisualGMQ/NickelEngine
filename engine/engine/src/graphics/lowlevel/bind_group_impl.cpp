#include "nickel/graphics/lowlevel/internal/bind_group_impl.hpp"

#include "nickel/graphics/lowlevel/internal/buffer_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"
#include "nickel/graphics/lowlevel/internal/sampler_impl.hpp"
#include "nickel/graphics/lowlevel/internal/vk_call.hpp"

namespace nickel::graphics {

BindGroupImpl::BindGroupImpl(DeviceImpl& dev, BindGroupLayoutImpl& layout,
                             const BindGroup::Descriptor& desc)
    : m_layout{&layout}, m_device{dev}, m_desc{desc} {
    writeDescriptors();
}

const BindGroup::Descriptor& BindGroupImpl::GetDescriptor() const {
    return m_desc;
}

VkDescriptorType cvtBufferType2DescriptorType(
    BindGroup::BufferBinding::Type type) {
    switch (type) {
        case BindGroup::BufferBinding::Type::Storage:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case BindGroup::BufferBinding::Type::Uniform:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // NOTE: Now we use VK_KHR_push_descriptor, we can use non-dynamic
        // m_buffer with m_offset to do same thing as dynamic m_buffer
        case BindGroup::BufferBinding::Type::DynamicStorage:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            // return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
        case BindGroup::BufferBinding::Type::DynamicUniform:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            // return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
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
    explicit WriteDescriptorHelper(DescriptorSetWriteInfo& write_info,
                                   uint32_t slot)
        : m_slot{slot}, m_write_info{write_info} {}

    void operator()(const BindGroup::BufferBinding& binding) const {
        if (binding.m_buffer.Impl().Size() == 0) {
            LOGW("m_buffer m_binding m_size == 0");
            return;
        }
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorBufferInfo buffer_info{};

        auto& buffer_impl = binding.m_buffer.Impl();
        buffer_info.buffer = buffer_impl.m_buffer;
        buffer_info.offset = binding.m_offset ? binding.m_offset.value() : 0;
        buffer_info.range =
            binding.m_size ? binding.m_size.value() : buffer_impl.Size();

        write_info.descriptorCount = 1;
        write_info.descriptorType = cvtBufferType2DescriptorType(binding.m_type);
        write_info.dstSet = VK_NULL_HANDLE;
        write_info.dstArrayElement = 0;
        write_info.pBufferInfo = &m_write_info.RecordBufferInfo(buffer_info);
        write_info.dstBinding = m_slot;

        m_write_info.RecordWriteDescriptor(write_info);
    }

    void operator()(const BindGroup::SamplerBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.sampler = binding.m_sampler.Impl().m_sampler;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &m_write_info.RecordImageInfo(image_info);
        write_info.dstArrayElement = 0;
        write_info.dstSet = VK_NULL_HANDLE;
        write_info.dstBinding = m_slot;
        write_info.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;

        m_write_info.RecordWriteDescriptor(write_info);
    }

    void operator()(const BindGroup::ImageBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.imageView = binding.m_view.Impl().m_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &m_write_info.RecordImageInfo(image_info);
        write_info.dstArrayElement = 0;
        write_info.dstSet = VK_NULL_HANDLE;
        write_info.dstBinding = m_slot;
        write_info.descriptorType = cvtImageType2DescriptorType(binding.m_type);

        m_write_info.RecordWriteDescriptor(write_info);
    }

    void operator()(const BindGroup::CombinedSamplerBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.imageView = binding.m_view.Impl().m_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.sampler = binding.m_sampler.Impl().m_sampler;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &m_write_info.RecordImageInfo(image_info);
        write_info.dstArrayElement = 0;
        write_info.dstSet = VK_NULL_HANDLE;
        write_info.dstBinding = m_slot;
        write_info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        m_write_info.RecordWriteDescriptor(write_info);
    }

private:
    uint32_t m_slot{};
    DescriptorSetWriteInfo& m_write_info;
};

void BindGroupImpl::writeDescriptors() {
    for (auto&& [slot, entry] : m_desc.m_entries) {
        WriteDescriptorHelper helper{m_write_infos, slot};
        std::visit(helper, entry.m_binding.m_entry);
    }
}

void BindGroupImpl::PendingDelete() {
    m_layout->m_bind_group_allocator.MarkAsGarbage(this);
}

const DescriptorSetWriteInfo& BindGroupImpl::GetWriteInfo() const {
    return m_write_infos;
}

}  // namespace nickel::graphics