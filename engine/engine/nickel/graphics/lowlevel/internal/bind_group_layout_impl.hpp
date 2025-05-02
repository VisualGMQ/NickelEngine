#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

class DeviceImpl;
class BindGroupPool;

class BindGroupLayoutImpl final : public RefCountable {
public:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;

    BindGroupLayoutImpl(DeviceImpl& dev,
                        const BindGroupLayout::Descriptor& desc,
                        BindGroupPool& pool, uint32_t descriptor_set_count);
    BindGroupLayoutImpl(const BindGroupLayoutImpl&) = delete;
    BindGroupLayoutImpl(BindGroupLayoutImpl&&) = delete;
    BindGroupLayoutImpl& operator=(const BindGroupLayoutImpl&) = delete;
    BindGroupLayoutImpl& operator=(BindGroupLayoutImpl&&) = delete;

    ~BindGroupLayoutImpl();

    BindGroup RequireBindGroup(const BindGroup::Descriptor& desc);

    void DecRefcount() override;
    void GC();
    void RecycleBindGroup(const BindGroupImpl&);

    BlockMemoryAllocator<BindGroupImpl> m_bind_group_allocator;
    std::vector<VkDescriptorSet> m_descriptor_sets;

private:
    DeviceImpl& m_device;
    std::vector<uint32_t> m_unused_descriptor_set;

    VkDescriptorSetLayoutBinding getBinding(uint32_t slot,
                                            const BindGroupLayout::Entry&);

    VkDescriptorSetLayout createLayout(DeviceImpl&,
                                       const BindGroupLayout::Descriptor&);
    void createSets(DeviceImpl&, VkDescriptorPool,
                    uint32_t descriptor_set_count);
};

}  // namespace nickel::graphics