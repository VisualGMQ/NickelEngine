#pragma once
#include "nickel/graphics/lowlevel/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/lowlevel/bind_group.hpp"

namespace nickel::graphics {

class DeviceImpl;

class BindGroupImpl {
public:
    BindGroupImpl(DeviceImpl&, size_t group_index, VkDescriptorSet,
                  BindGroupLayoutImpl& layout, const BindGroup::Descriptor&);
    BindGroupImpl(const BindGroupImpl&) = delete;
    BindGroupImpl(BindGroupImpl&&) = delete;
    BindGroupImpl& operator=(const BindGroupImpl&) = delete;
    BindGroupImpl& operator=(BindGroupImpl&&) = delete;

    ~BindGroupImpl();

    const BindGroup::Descriptor& GetDescriptor() const;
    void WriteDescriptors();
    void PendingDelete();

    BindGroupLayoutImpl& m_layout;
    VkDescriptorSet m_set = VK_NULL_HANDLE;

    size_t m_group_index{};

private:
    DeviceImpl& m_device;
    BindGroup::Descriptor m_desc;
};

}  // namespace nickel::graphics