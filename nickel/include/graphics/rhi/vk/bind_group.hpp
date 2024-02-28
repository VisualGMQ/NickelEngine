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

    auto& Descriptor() const { return desc_; }

    vk::DescriptorSetLayout layout;
    vk::DescriptorPool pool;
    std::vector<vk::DescriptorSet> sets;

private:
    DeviceImpl& device_;
    BindGroupLayout::Descriptor desc_;

    vk::DescriptorSetLayoutBinding getBinding(const Entry&);

    void createPool(vk::Device dev, uint32_t imageCount,
                    const BindGroupLayout::Descriptor& desc);
    void allocSets(vk::Device dev, uint32_t count,
                   const BindGroupLayout::Descriptor& desc);
};

class BindGroupImpl : public rhi::BindGroupImpl {
public:
    BindGroupImpl(DeviceImpl&, const BindGroup::Descriptor&);

    BindGroupLayout GetLayout() const { return layout_; }

    std::vector<vk::DescriptorSet> sets;

    void WriteDescriptors();

private:
    DeviceImpl& device_;
    BindGroupLayout layout_;
    BindGroup::Descriptor desc_;
};

}  // namespace nickel::rhi::vulkan