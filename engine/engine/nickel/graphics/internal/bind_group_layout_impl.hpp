#pragma once
#include "nickel/graphics/bind_group_layout.hpp"
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

using DescriptorSetLists = std::vector<VkDescriptorSet>;

class DeviceImpl;

class BindGroupLayoutImpl final : public RefCountable {
public:
    BindGroupLayoutImpl(DeviceImpl&, const BindGroupLayout::Descriptor&);
    ~BindGroupLayoutImpl();

    VkDescriptorSetLayout m_layout;
    VkDescriptorPool m_pool;
    std::vector<DescriptorSetLists> m_groups;

    const DescriptorSetLists* RequireSetList();
    void RecycleSetList(size_t index);

private:
    VkDevice m_device;
    size_t m_group_elem_count;
    std::vector<size_t> m_unused_group_indices;

    VkDescriptorSetLayoutBinding getBinding(uint32_t slot,
                                            const BindGroupLayout::Entry&);
    void allocSets(uint32_t count, const BindGroupLayout::Descriptor& desc);
    void createPool(uint32_t count, const BindGroupLayout::Descriptor& desc);
};

}  // namespace nickel::graphics