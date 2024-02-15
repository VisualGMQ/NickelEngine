#include "graphics/rhi/null/device.hpp"
#include "graphics/rhi/null/adapter.hpp"
#include "graphics/rhi/null/command.hpp"
#include "graphics/rhi/null/queue.hpp"
#include "graphics/rhi/null/render_pipeline.hpp"
#include "graphics/rhi/null/texture.hpp"

namespace nickel::rhi::null {

Texture DeviceImpl::CreateTexture(const Texture::Descriptor& desc) {
    AdapterImpl adapter;
    return Texture{adapter, *this, {}, {}};
}

RenderPipeline DeviceImpl::CreateRenderPipeline(
    const RenderPipeline::Descriptor&) {
    return RenderPipeline{APIPreference::Null, *this, {}};
}

PipelineLayout DeviceImpl::CreatePipelineLayout(
    const PipelineLayout::Descriptor&) {
    return PipelineLayout{APIPreference::Null, *this, {}};
}

Sampler DeviceImpl::CreateSampler(const Sampler::Descriptor&) {
    return Sampler{APIPreference::Null, *this, {}};
}

CommandEncoder DeviceImpl::CreateCommandEncoder() {
    return CommandEncoder{new CommandEncoderImpl{}};
}

BindGroup DeviceImpl::CreateBindGroup(const BindGroup::Descriptor& desc) {
    return BindGroup{APIPreference::Null, *this, {}};
}

BindGroupLayout DeviceImpl::CreateBindGroupLayout(
    const BindGroupLayout::Descriptor& desc) {
    return BindGroupLayout{APIPreference::Null, *this, {}};
}

ShaderModule DeviceImpl::CreateShaderModule(const ShaderModule::Descriptor&) {
    return ShaderModule{APIPreference::Null, *this, {}};
}

Buffer DeviceImpl::CreateBuffer(const Buffer::Descriptor&) {
    AdapterImpl adapter;
    return Buffer{adapter, *this, {}};
}

Queue DeviceImpl::GetQueue() {
    return Queue{new QueueImpl{}};
}

void DeviceImpl::SwapContext() {}

void DeviceImpl::WaitIdle() {}

}  // namespace nickel::rhi::null