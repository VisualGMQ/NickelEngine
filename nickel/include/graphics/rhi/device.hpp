#pragma once

#include "graphics/rhi/command.hpp"
#include "graphics/rhi/queue.hpp"
#include "graphics/rhi/render_pipeline.hpp"
#include "graphics/rhi/renderpass.hpp"
#include "graphics/rhi/texture.hpp"
namespace nickel::rhi {

class DeviceImpl;
class Adapter;

class Device {
public:
    Device() = default;
    explicit Device(AdapterImpl& adapter);

    Texture CreateTexture(const Texture::Descriptor& desc);
    RenderPipeline CreateRenderPipeline(const RenderPipeline::Descriptor&);
    PipelineLayout CreatePipelineLayout(const PipelineLayout::Descriptor&);
    Sampler CreateSampler(const Sampler::Descriptor&);
    CommandEncoder CreateCommandEncoder();
    BindGroup CreateBindGroup(const BindGroup::Descriptor&);
    BindGroupLayout CreateBindGroupLayout(const BindGroupLayout::Descriptor&);
    ShaderModule CreateShaderModule(const ShaderModule::Descriptor& desc);

    void SwapContext();
    void WaitIdle();
    Queue GetQueue();

    void Destroy();

private:
    DeviceImpl* impl_{};
};

}