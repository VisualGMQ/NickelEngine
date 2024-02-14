#pragma once

#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/impl/bind_group.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;

class BindGroupLayoutImpl : public rhi::BindGroupLayoutImpl {
public:
    BindGroupLayoutImpl(DeviceImpl&, const BindGroupLayout::Descriptor&);
    ~BindGroupLayoutImpl();

    vk::DescriptorSetLayout layout;

private:
    vk::Device device_;

    vk::DescriptorSetLayoutBinding getBinding(const Entry&);
};

class BindGroupImpl : public rhi::BindGroupImpl {
public:
    BindGroupImpl(DeviceImpl&, const BindGroup::Descriptor&);
    ~BindGroupImpl();

    BindGroupLayout GetLayout() const { return layout_; }

    vk::DescriptorPool pool;
    std::vector<vk::DescriptorSet> sets;

private:
    DeviceImpl& device_;
    BindGroupLayout layout_;

    void createPool(vk::Device dev, uint32_t imageCount,
                    const BindGroup::Descriptor& desc);
    void allocSets(vk::Device dev, uint32_t imageCount,
                   const BindGroup::Descriptor& desc);
    void writeDescriptors(vk::Device dev, const BindGroup::Descriptor&);
};

}  // namespace nickel::rhi::vulkan