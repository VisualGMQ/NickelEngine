#pragma once

#include "graphics/rhi/impl/device.hpp"

namespace nickel::rhi::null {

class DeviceImpl: public rhi::DeviceImpl {
public:
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
    std::pair<Texture, TextureView> GetPresentationTexture() override;

    void BeginFrame() override;
    void EndFrame() override;
    void WaitIdle() override;
};

};