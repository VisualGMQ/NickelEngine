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
    RenderPipelineImpl(DeviceImpl&, const RenderPipeline::Descriptor&,
                       vk::RenderPass);
    RenderPipelineImpl(RenderPipelineImpl&&);
    ~RenderPipelineImpl();

    RenderPipelineImpl& operator=(RenderPipelineImpl&&);

    PipelineLayout GetLayout() const { return layout_; }

    const RenderPipeline::Descriptor& GetDescriptor() const;

    vk::Pipeline pipeline;
    vk::RenderPass defaultRenderPass;

private:
    PipelineLayout layout_;
    vk::Device dev_;
    RenderPipeline::Descriptor desc_;
    std::vector<vk::ShaderModule> shaderModules_;

    void createRenderPipeline(DeviceImpl&, const RenderPipeline::Descriptor&,
                              vk::RenderPass renderPass);
    void createRenderPass(DeviceImpl&, const RenderPipeline::Descriptor&);

    friend void swap(RenderPipelineImpl& o1, RenderPipelineImpl& o2) noexcept {
        using std::swap;
        swap(o1.pipeline, o2.pipeline);
        swap(o1.defaultRenderPass, o2.defaultRenderPass);
        swap(o1.layout_, o2.layout_);
        swap(o1.dev_, o2.dev_);
        swap(o1.desc_, o2.desc_);
        swap(o1.shaderModules_, o2.shaderModules_);
    }
};

}  // namespace nickel::rhi::vulkan