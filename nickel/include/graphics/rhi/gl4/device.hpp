#pragma once

#include "graphics/rhi/impl/device.hpp"
#include "graphics/rhi/gl4/shader.hpp"
#include "graphics/rhi/gl4/sampler.hpp"
#include "graphics/rhi/gl4/bind_group.hpp"
#include "graphics/rhi/gl4/render_pipeline.hpp"
#include "graphics/rhi/gl4/pipeline_layout.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/command.hpp"
#include "graphics/rhi/gl4/queue.hpp"

namespace nickel::rhi::gl4 {

class AdapterImpl;

class DeviceImpl: public rhi::DeviceImpl {
public:
    explicit DeviceImpl(AdapterImpl& adapter);
    ~DeviceImpl();

    Texture CreateTexture(const Texture::Descriptor& desc) override;
    RenderPipeline CreateRenderPipeline(
        const RenderPipeline::Descriptor&) override;
    PipelineLayout CreatePipelineLayout(
        const PipelineLayout::Descriptor&) override;
    Sampler CreateSampler(const Sampler::Descriptor&) override;
    CommandEncoder CreateCommandEncoder() override;
    BindGroup CreateBindGroup(const BindGroup::Descriptor& desc) override;
    BindGroupLayout CreateBindGroupLayout(
        const BindGroupLayout::Descriptor& desc) override;
    ShaderModule CreateShaderModule(
        const ShaderModule::Descriptor&) override;
    Buffer CreateBuffer(const Buffer::Descriptor&) override;
    Queue GetQueue() override;

    void SwapContext() override;
    void WaitIdle() override;

    Queue queue;

    GLuint swapchainTexture;
    GLuint swapchainFramebuffer;

    std::vector<Framebuffer> framebuffers;
    std::unordered_map<size_t, GLuint> vaos;  // index vao by indices-buffer

    AdapterImpl* adapter;
};

}