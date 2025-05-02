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

class BindGroupImpl: public RefCountable {
public:
    BindGroupImpl(DeviceImpl&, BindGroupLayoutImpl& layout,
                  const BindGroup::Descriptor&,
                  VkDescriptorSet descriptor_set,
                  uint32_t id);
    BindGroupImpl(const BindGroupImpl&) = delete;
    BindGroupImpl(BindGroupImpl&&) = delete;
    BindGroupImpl& operator=(const BindGroupImpl&) = delete;
    BindGroupImpl& operator=(BindGroupImpl&&) = delete;

    const BindGroup::Descriptor& GetDescriptor() const;
    void DecRefcount() override;

    BindGroupLayout m_layout{};
    VkDescriptorSet m_descriptor_set;

    uint32_t GetID() const;

private:
    uint32_t m_id;
    DeviceImpl& m_device;
    BindGroup::Descriptor m_desc;

    void writeDescriptors(const BindGroup::Descriptor&) const;
};

}  // namespace nickel::graphics