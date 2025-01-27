#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;

class BindGroupLayoutImpl final : public RefCountable {
public:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_groups;
    BlockMemoryAllocator<BindGroupImpl> m_bind_group_allocator;

    BindGroupLayoutImpl(DeviceImpl&, const BindGroupLayout::Descriptor&);
    BindGroupLayoutImpl(const BindGroupLayoutImpl&) = delete;
    BindGroupLayoutImpl(BindGroupLayoutImpl&&) = delete;
    BindGroupLayoutImpl& operator=(const BindGroupLayoutImpl&) = delete;
    BindGroupLayoutImpl& operator=(BindGroupLayoutImpl&&) = delete;

    ~BindGroupLayoutImpl();

    BindGroup RequireBindGroup(const BindGroup::Descriptor& desc);

    VkDescriptorSet RequireSet(size_t& out_idx);
    void RecycleSetList(size_t index);
    void DecRefcount() override;

private:
    DeviceImpl& m_device;
    std::vector<size_t> m_unused_group_indices;

    VkDescriptorSetLayoutBinding getBinding(uint32_t slot,
                                            const BindGroupLayout::Entry&);
    void allocSets(uint32_t count);
    void createPool(uint32_t count, const BindGroupLayout::Descriptor& desc);
};

}  // namespace nickel::graphics