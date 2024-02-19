#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/gl4/adapter.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/queue.hpp"

namespace nickel::rhi::gl4 {

DeviceImpl::DeviceImpl(AdapterImpl& adapter)
    : queue{new QueueImpl{}}, adapter_(&adapter) {}

Texture DeviceImpl::CreateTexture(const Texture::Descriptor& desc) {
    return Texture{*adapter_, *this, desc, {}};
}

RenderPipeline DeviceImpl::CreateRenderPipeline(
    const RenderPipeline::Descriptor& desc) {
    return RenderPipeline{APIPreference::GL, *this, desc};
}

PipelineLayout DeviceImpl::CreatePipelineLayout(
    const PipelineLayout::Descriptor& desc) {
    return PipelineLayout{APIPreference::GL, *this, desc};
}

Sampler DeviceImpl::CreateSampler(const Sampler::Descriptor& desc) {
    return Sampler{APIPreference::GL, *this, desc};
}

CommandEncoder DeviceImpl::CreateCommandEncoder() {
    return CommandEncoder{new CommandEncoderImpl{}};
}

BindGroup DeviceImpl::CreateBindGroup(const BindGroup::Descriptor& desc) {
    return BindGroup{APIPreference::GL, *this, desc};
}

BindGroupLayout DeviceImpl::CreateBindGroupLayout(
    const BindGroupLayout::Descriptor& desc) {
    return BindGroupLayout{APIPreference::GL, *this, desc};
}

ShaderModule DeviceImpl::CreateShaderModule(
    const ShaderModule::Descriptor& desc) {
    return ShaderModule{APIPreference::GL, *this, desc};
}

Buffer DeviceImpl::CreateBuffer(const Buffer::Descriptor& desc) {
    return Buffer{*adapter_, *this, desc};
}

Queue DeviceImpl::GetQueue() {
    return queue;
}

void DeviceImpl::SwapContext() {
    SDL_GL_SwapWindow(adapter_->window);
}

void DeviceImpl::WaitIdle() {
    GL_CALL(glFlush());
}

}  // namespace nickel::rhi::gl4