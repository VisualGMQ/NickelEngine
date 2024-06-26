#include "graphics/rhi/gl/device.hpp"
#include "graphics/rhi/gl/adapter.hpp"
#include "graphics/rhi/gl/glcall.hpp"
#include "graphics/rhi/gl/queue.hpp"

namespace nickel::rhi::gl {

DeviceImpl::DeviceImpl(AdapterImpl& adapter)
    : queue{new QueueImpl{*this}},
      adapter(&adapter) {
    initSwapchain();
    initPushConstantBuffer();
    presentTexture = std::make_unique<TextureImpl>(swapchainTexture, GL_TEXTURE_2D);
    presentTextureView = std::make_unique<TextureViewImpl>(
        *presentTexture, TextureView::Descriptor{});
}

void DeviceImpl::initSwapchain() {
    GL_CALL(glGenTextures(1, &swapchainTexture));
    int w, h;
    SDL_GetWindowSize(adapter->window, &w, &h);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, swapchainTexture));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                         GL_UNSIGNED_BYTE, 0));
    GL_CALL(glGenFramebuffers(1, &swapchainFramebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, swapchainFramebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   GL_TEXTURE_2D, swapchainTexture, 0));
}

void DeviceImpl::initPushConstantBuffer() {
    GL_CALL(glGenBuffers(1, &pushConstantBuf));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pushConstantBuf));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, adapter->Limits().maxPushConstantSize,
                         0, GL_DYNAMIC_DRAW));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

DeviceImpl::~DeviceImpl() {
    GL_CALL(glDeleteTextures(1, &swapchainTexture));
    GL_CALL(glDeleteBuffers(1, &pushConstantBuf));
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

std::pair<Texture, TextureView> DeviceImpl::GetPresentationTexture() {
    return {Texture{presentTexture.get()}, TextureView{presentTextureView.get()}};
}

void DeviceImpl::BeginFrame() {}

void DeviceImpl::EndFrame() {
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, swapchainFramebuffer));
    GL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    int w, h;
    SDL_GetWindowSize(adapter->window, &w, &h);
    GL_CALL(glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT,
                              GL_LINEAR));
    SDL_GL_SwapWindow(adapter->window);
}

void DeviceImpl::OnWindowResize(int x, int y) {}

void DeviceImpl::WaitIdle() {
    GL_CALL(glFlush());
}

}  // namespace nickel::rhi::gl