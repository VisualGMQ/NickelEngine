#include "graphics/rhi/vk/pipeline_layout.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/bind_group.hpp"

namespace nickel::rhi::vulkan {

PipelineLayoutImpl::PipelineLayoutImpl(DeviceImpl& dev, const std::vector<vk::DescriptorSetLayout>& layouts,
const std::vector<vk::PushConstantRange>& ranges): dev_{dev.device} {
    vk::PipelineLayoutCreateInfo info;
    info.setPushConstantRanges(ranges)
        .setSetLayouts(layouts);
    VK_CALL(layout, dev.device.createPipelineLayout(info));
}

PipelineLayoutImpl::~PipelineLayoutImpl() {
    dev_.destroyPipelineLayout(layout);
}

}