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

    BindGroupLayoutImpl(DeviceImpl&, const BindGroupLayout::Descriptor&);
    BindGroupLayoutImpl(const BindGroupLayoutImpl&) = delete;
    BindGroupLayoutImpl(BindGroupLayoutImpl&&) = delete;
    BindGroupLayoutImpl& operator=(const BindGroupLayoutImpl&) = delete;
    BindGroupLayoutImpl& operator=(BindGroupLayoutImpl&&) = delete;

    ~BindGroupLayoutImpl();

    BindGroup RequireBindGroup(const BindGroup::Descriptor& desc);

    void DecRefcount() override;
    void GC();

    BlockMemoryAllocator<BindGroupImpl> m_bind_group_allocator;

private:
    DeviceImpl& m_device;

    VkDescriptorSetLayoutBinding getBinding(uint32_t slot,
                                            const BindGroupLayout::Entry&);
};

}  // namespace nickel::graphics