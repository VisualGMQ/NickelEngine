#include "nickel/graphics/internal/bind_group_impl.hpp"

#include "nickel/graphics/internal/buffer_impl.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/graphics/internal/image_view_impl.hpp"
#include "nickel/graphics/internal/sampler_impl.hpp"
#include "nickel/graphics/internal/vk_call.hpp"

namespace nickel::graphics {

BindGroupImpl::BindGroupImpl(DeviceImpl& dev, size_t group_index,
                             DescriptorSetLists& set_lists,
                             BindGroupLayoutImpl& layout,
                             const BindGroup::Descriptor& desc)
    : m_layout{layout},
      m_set_lists{set_lists},
      m_group_index{group_index},
      m_device{dev},
      m_desc{desc} {}

BindGroupImpl::~BindGroupImpl() {
    m_layout.RecycleSetList(m_group_index);
}

const BindGroup::Descriptor& BindGroupImpl::GetDescriptor() const {
    return m_desc;
}

VkDescriptorType cvtBufferType2DescriptorType(BindGroup::BufferBinding::Type type) {
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

VkDescriptorType cvtImageType2DescriptorType(BindGroup::ImageBinding::Type type) {
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
    WriteDescriptorHelper(VkDescriptorSet set, DeviceImpl& dev,
                          const BindGroup::BindingPoint& binding)
        : m_set{set}, m_dev{dev}, m_bind_point{binding} {}

    void operator()(const BindGroup::BufferBinding& binding) const {
        if (binding.buffer.Impl().Size() == 0) {
            LOGW("buffer binding size == 0");
            return;
        }
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorBufferInfo buffer_info{};

        auto& buffer_impl = binding.buffer.Impl();
        buffer_info.buffer = buffer_impl.m_buffer;
        buffer_info.offset = binding.offset ? binding.offset.value() : 0;
        buffer_info.range =
            binding.size ? binding.size.value() : buffer_impl.Size();

        write_info.descriptorCount = 1;
        write_info.descriptorType = cvtBufferType2DescriptorType(binding.type);
        write_info.dstSet = m_set;
        write_info.dstArrayElement = 0;
        write_info.pBufferInfo = &buffer_info;
        write_info.dstBinding = m_bind_point.slot;

        vkUpdateDescriptorSets(m_dev.m_device, 1, &write_info, 1, nullptr);
    }

    void operator()(const BindGroup::SamplerBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.sampler = binding.sampler.Impl().m_sampler;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &image_info;
        write_info.dstArrayElement = 0;
        write_info.dstSet = m_set;
        write_info.dstBinding = m_bind_point.slot;
        write_info.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;

        vkUpdateDescriptorSets(m_dev.m_device, 1, &write_info, 1, nullptr);
    }

    void operator()(const BindGroup::ImageBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.imageView = binding.view.Impl().m_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &image_info;
        write_info.dstArrayElement = 0;
        write_info.dstSet = m_set;
        write_info.dstBinding = m_bind_point.slot;
        write_info.descriptorType = cvtImageType2DescriptorType(binding.type);

        vkUpdateDescriptorSets(m_dev.m_device, 1, &write_info, 1, nullptr);
    }

    void operator()(const BindGroup::CombinedSamplerBinding& binding) const {
        VkWriteDescriptorSet write_info{};
        write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        VkDescriptorImageInfo image_info{};
        image_info.imageView = binding.view.Impl().m_view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.sampler = binding.sampler.Impl().m_sampler;

        write_info.descriptorCount = 1;
        write_info.pImageInfo = &image_info;
        write_info.dstArrayElement = 0;
        write_info.dstSet = m_set;
        write_info.dstBinding = m_bind_point.slot;
        write_info.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

        vkUpdateDescriptorSets(m_dev.m_device, 1, &write_info, 1, nullptr);
    }

private:
    VkDescriptorSet m_set;
    DeviceImpl& m_dev;
    const BindGroup::BindingPoint& m_bind_point;
};

void BindGroupImpl::WriteDescriptors() {
    size_t i = 0;
    for (auto&& [slot, entry] : m_desc.entries) {
        WriteDescriptorHelper helper{m_set_lists[i++], m_device, entry.binding};
        std::visit(helper, entry.binding.entry);
    }
}

}  // namespace nickel::graphics