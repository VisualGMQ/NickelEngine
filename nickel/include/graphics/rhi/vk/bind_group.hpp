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

    std::vector<vk::DescriptorSet> RequireSets(uint32_t id);
    uint32_t RequireBindGroupID();
    void DestoryID(uint32_t id);

private:
    std::vector<bool> ids_;
    DeviceImpl& device_;
    BindGroupLayout::Descriptor desc_;

    vk::DescriptorSetLayoutBinding getBinding(const Entry&);

    void createPool(vk::Device dev, uint32_t count,
                    const BindGroupLayout::Descriptor& desc);
    void allocSets(vk::Device dev, uint32_t count,
                   const BindGroupLayout::Descriptor& desc);
};

class BindGroupImpl : public rhi::BindGroupImpl {
public:
    BindGroupImpl(DeviceImpl&, const BindGroup::Descriptor&);
    ~BindGroupImpl();

    BindGroupLayout GetLayout() const { return desc_.layout; }

    void Transformlayouts();

    std::vector<vk::DescriptorSet> sets;

private:
    uint32_t id_;
    DeviceImpl& device_;
    BindGroup::Descriptor desc_;
    std::vector<std::function<void(void)>> layoutTransfers_;

    void writeDescriptors();
};

}  // namespace nickel::rhi::vulkan