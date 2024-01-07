#pragma once

#include "rhi/descriptor.hpp"
#include "rhi/vulkan/util.hpp"
#include "rhi/vulkan/enum_convert.hpp"

namespace nickel::rhi::vulkan {

inline vk::DescriptorSetLayoutBinding DescriptorSetLayoutBinding2Vk(
    const DescriptorSetLayoutBinding& binding) {
    vk::DescriptorSetLayoutBinding b;
    b.setBinding(binding.binding)
        .setDescriptorCount(binding.count)
        .setDescriptorType(DescriptorType2Vk(binding.type))
        .setStageFlags(ShaderStageFlags2Vk(binding.shaderStage));
    // TODO: add sampler to it
    return b;
}

inline vk::PushConstantRange PushConstantRange2Vk(
    const PushConstantRange& range) {
    vk::PushConstantRange r;
    r.setOffset(range.offset)
        .setSize(range.size)
        .setStageFlags(ShaderStageFlags2Vk(range.shaderStage));
    return r;
}

class Device;

class DescriptorSetLayout : public rhi::DescriptorLayout {
public:
    explicit DescriptorSetLayout(
        Device* device,
        const std::vector<DescriptorSetLayoutBinding>& bindings);
    virtual ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

    DescriptorSetLayout(DescriptorSetLayout&& o) { swap(*this, o); }

    DescriptorSetLayout& operator=(DescriptorSetLayout&& o) {
        if (&o != this) {
            swap(*this, o);
        }
        return *this;
    }

    auto& Raw() { return layout_; }

private:
    Device* device_ = nullptr;
    vk::DescriptorSetLayout layout_;

    friend void swap(DescriptorSetLayout& o1,
                     DescriptorSetLayout& o2) noexcept {
        using std::swap;
        swap(o1.device_, o2.device_);
        swap(o1.layout_, o2.layout_);
    }
};

}  // namespace nickel::rhi::vulkan