#pragma once
#include "nickel/graphics/internal/bind_group_layout_impl.hpp"
#include "nickel/graphics/bind_group.hpp"

namespace nickel::graphics {

class DeviceImpl;

class BindGroupImpl {
public:
    BindGroupImpl(DeviceImpl&, size_t group_index, VkDescriptorSet,
                  BindGroupLayoutImpl& layout, const BindGroup::Descriptor&);
    ~BindGroupImpl();
    
    const BindGroup::Descriptor& GetDescriptor() const;
    void WriteDescriptors();
    void Delete();

    BindGroupLayoutImpl& m_layout;
    VkDescriptorSet m_set;

    size_t m_group_index{};

private:
    DeviceImpl& m_device;
    BindGroup::Descriptor m_desc;
};

}  // namespace nickel::graphics