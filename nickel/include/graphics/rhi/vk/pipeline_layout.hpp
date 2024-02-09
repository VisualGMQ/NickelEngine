#pragma once

#include "graphics/rhi/impl/pipeline_layout.hpp"
#include "graphics/rhi/pipeline_layout.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;

class PipelineLayoutImpl: public rhi::PipelineLayoutImpl {
public:
    PipelineLayoutImpl(DeviceImpl&, const std::vector<vk::DescriptorSetLayout>&,
                       const std::vector<vk::PushConstantRange>&);
    ~PipelineLayoutImpl();

    vk::PipelineLayout layout;

private:
    vk::Device dev_;
};

}