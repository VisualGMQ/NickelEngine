#pragma once

#include "graphics/rhi/impl/render_pipeline.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/pipeline_layout.hpp"
#include "graphics/rhi/vk/util.hpp"


namespace nickel::rhi::vulkan {

class DeviceImpl;

class RenderPipelineImpl : public rhi::RenderPipelineImpl {
public:
    RenderPipelineImpl(DeviceImpl&, const RenderPipeline::Descriptor&);
    ~RenderPipelineImpl();

    PipelineLayout GetLayout() const { return layout_; }

    vk::Pipeline pipeline;
    vk::RenderPass renderPass;

private:
    PipelineLayout layout_;
    vk::Device dev_;

    void createRenderPipeline(DeviceImpl&, const RenderPipeline::Descriptor&);
    void createRenderPass(DeviceImpl&, const RenderPipeline::Descriptor&);
};

}  // namespace nickel::rhi::vulkan