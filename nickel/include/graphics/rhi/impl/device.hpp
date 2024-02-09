#pragma once

#include "graphics/rhi/command.hpp"
#include "graphics/rhi/queue.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/texture.hpp"
namespace nickel::rhi {

class DeviceImpl {
public:
    virtual Texture CreateTexture(const Texture::Descriptor& desc) = 0;
    virtual RenderPipeline CreateRenderPipeline(
        const RenderPipeline::Descriptor&) = 0;
    virtual PipelineLayout CreatePipelineLayout(
        const PipelineLayout::Descriptor&) = 0;
    virtual Sampler CreateSampler(const Sampler::Descriptor&) = 0;
    virtual CommandEncoder CreateCommandEncoder() = 0;
    virtual BindGroup CreateBindGroup(const BindGroup::Descriptor& desc) = 0;
    virtual BindGroupLayout CreateBindGroupLayout(
        const BindGroupLayout::Descriptor& desc) = 0;
    virtual ShaderModule CreateShaderModule(
        const ShaderModule::Descriptor&) = 0;
    virtual Queue GetQueue() = 0;

    virtual void SwapContext() = 0;
    virtual void WaitIdle() = 0;

    virtual ~DeviceImpl() = default;
};

}  // namespace nickel::rhi