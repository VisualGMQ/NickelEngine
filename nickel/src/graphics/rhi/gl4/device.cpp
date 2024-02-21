#include "graphics/rhi/gl4/device.hpp"
#include "graphics/rhi/gl4/adapter.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/queue.hpp"

namespace nickel::rhi::gl4 {

DeviceImpl::DeviceImpl(AdapterImpl& adapter)
    : queue{new QueueImpl{}}, adapter(&adapter) {
    GL_CALL(glGenTextures(1, &swapchainTexture));
    int w, h;
    SDL_GetWindowSize(adapter.window, &w, &h);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, swapchainTexture));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
    GL_CALL(glGenFramebuffers(1, &swapchainFramebuffer));
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, swapchainFramebuffer));
    GL_CALL(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, swapchainTexture, 0));
}

DeviceImpl::~DeviceImpl() {
    GL_CALL(glDeleteTextures(1, &swapchainTexture));
    for (auto fbo : framebuffers) {
        fbo.Destroy();
    }
    framebuffers.clear();
    queue.Destroy();
}

Texture DeviceImpl::CreateTexture(const Texture::Descriptor& desc) {
    return Texture{*adapter, *this, desc, {}};
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
    return CommandEncoder{new CommandEncoderImpl{*this}};
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
    return Buffer{*adapter, *this, desc};
}

Queue DeviceImpl::GetQueue() {
    return queue;
}

void DeviceImpl::SwapContext() {
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, swapchainFramebuffer));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    int w, h;
    SDL_GetWindowSize(adapter->window, &w, &h);
    GL_CALL(glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT,
                              GL_NEAREST));
    SDL_GL_SwapWindow(adapter->window);
}

void DeviceImpl::WaitIdle() {
    GL_CALL(glFlush());
}

}  // namespace nickel::rhi::gl4