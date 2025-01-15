#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/bind_group.hpp"
#include "nickel/graphics/bind_group_layout.hpp"
#include "nickel/graphics/internal/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;

class BindGroupLayoutImpl final : public RefCountable {
public:
    VkDescriptorSetLayout m_layout;
    VkDescriptorPool m_pool;
    std::vector<VkDescriptorSet> m_groups;
    BlockMemoryAllocator<BindGroupImpl> m_bind_group_allocator;
    
    BindGroupLayoutImpl(DeviceImpl&, const BindGroupLayout::Descriptor&);
    ~BindGroupLayoutImpl();

    BindGroup RequireBindGroup(const BindGroup::Descriptor& desc);

    VkDescriptorSet RequireSet(size_t& out_idx);
    void RecycleSetList(size_t index);
    void PendingDelete();

private:
    DeviceImpl& m_device;
    std::vector<size_t> m_unused_group_indices;

    VkDescriptorSetLayoutBinding getBinding(uint32_t slot,
                                            const BindGroupLayout::Entry&);
    void allocSets(uint32_t count);
    void createPool(uint32_t count, const BindGroupLayout::Descriptor& desc);
};

}  // namespace nickel::graphics