#pragma once
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_layout_impl.hpp"
#include <forward_list>

namespace nickel::graphics {

class DeviceImpl;

struct DescriptorSetWriteInfo {
    const std::vector<VkWriteDescriptorSet> GetWriteDescriptorSets() const {
        return m_write_descriptor_sets;
    }

    void RecordWriteDescriptor(const VkWriteDescriptorSet& set) {
        m_write_descriptor_sets.emplace_back(set);
    }

    const VkDescriptorBufferInfo& RecordBufferInfo(
        const VkDescriptorBufferInfo& buffer_info) {
        return m_buffer_infos.emplace_front(buffer_info);
    }

    const VkDescriptorImageInfo& RecordImageInfo(
        const VkDescriptorImageInfo& info) {
        return m_image_infos.emplace_front(info);
    }

private:
    std::vector<VkWriteDescriptorSet> m_write_descriptor_sets;
    std::forward_list<VkDescriptorBufferInfo> m_buffer_infos;
    std::forward_list<VkDescriptorImageInfo> m_image_infos;
};

class BindGroupImpl {
public:
    BindGroupImpl(DeviceImpl&, BindGroupLayoutImpl& layout,
                  const BindGroup::Descriptor&);
    BindGroupImpl(const BindGroupImpl&) = delete;
    BindGroupImpl(BindGroupImpl&&) = delete;
    BindGroupImpl& operator=(const BindGroupImpl&) = delete;
    BindGroupImpl& operator=(BindGroupImpl&&) = delete;

    const BindGroup::Descriptor& GetDescriptor() const;
    void PendingDelete();

    BindGroupLayoutImpl* m_layout{};

    const DescriptorSetWriteInfo& GetWriteInfo() const;

private:
    DeviceImpl& m_device;
    BindGroup::Descriptor m_desc;
    DescriptorSetWriteInfo m_write_infos;

    void writeDescriptors();
};

}  // namespace nickel::graphics