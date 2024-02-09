#include "graphics/rhi/pipeline_layout.hpp"
#include "graphics/rhi/vk/bind_group.hpp"
#include "graphics/rhi/vk/convert.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/pipeline_layout.hpp"

namespace nickel::rhi {

PipelineLayout::PipelineLayout(APIPreference api, DeviceImpl& dev,
                               const Descriptor& desc) {
    switch (api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan: {
            std::vector<vk::DescriptorSetLayout> layouts;
            for (auto& layout : desc.layouts) {
                layouts.emplace_back(
                    static_cast<vulkan::BindGroupLayoutImpl*>(layout.impl_)
                        ->layout);
            }
            std::vector<vk::PushConstantRange> ranges;
            for (auto& range : desc.pushConstRanges) {
                vk::PushConstantRange constRange;
                constRange.setOffset(range.offset)
                    .setSize(range.size)
                    .setStageFlags(vulkan::ShaderStage2Vk(range.stage));
                ranges.emplace_back(constRange);
            }
            impl_ = new vulkan::PipelineLayoutImpl(
                static_cast<vulkan::DeviceImpl&>(dev), layouts, ranges);
        } break;
    }
}

void PipelineLayout::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}

}  // namespace nickel::rhi