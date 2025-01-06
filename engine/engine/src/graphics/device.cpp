#include "nickel/graphics/device.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/graphics/internal/device_impl.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel::graphics {

Device::Device(const AdapterImpl& adapter_impl,
               const SVector<uint32_t, 2>& window_size)
    : m_impl{std::make_unique<DeviceImpl>(adapter_impl, window_size)} {}

Device::~Device() {}

DeviceImpl& Device::Impl() {
    return *m_impl;
}

const DeviceImpl& Device::Impl() const {
    return *m_impl;
}

Buffer Device::CreateBuffer(const Buffer::Descriptor& desc) {
    return m_impl->CreateBuffer(desc);
}

Image Device::CreateImage(const Image::Descriptor& desc) {
    return m_impl->CreateImage(desc);
}

BindGroupLayout Device::CreateBindGroupLayout(
    const BindGroupLayout::Descriptor& desc) {
    return m_impl->CreateBindGroupLayout(desc);
}

PipelineLayout Device::CreatePipelineLayout(
    const PipelineLayout::Descriptor& desc) {
    return m_impl->CreatePipelineLayout(desc);
}

Framebuffer Device::CreateFramebuffer(const Framebuffer::Descriptor& desc) {
    return m_impl->CreateFramebuffer(desc);
}

RenderPass Device::CreateRenderPass(const RenderPass::Descriptor& desc) {
    return m_impl->CreateRenderPass(desc);
}

GraphicsPipeline Device::CreateGraphicPipeline(
    const GraphicsPipeline::Descriptor& desc) {
    return m_impl->CreateGraphicPipeline(desc);
}

Sampler Device::CreateSampler(const Sampler::Descriptor& desc) {
    return m_impl->CreateSampler(desc);
}

ShaderModule Device::CreateShaderModule(const uint32_t* data, size_t size) {
    return m_impl->CreateShaderModule(data, size);
}

Semaphore Device::CreateSemaphore() {
    return m_impl->CreateSemaphore();
}

Fence Device::CreateSemaphore(bool signaled) {
    return m_impl->CreateSemaphore(signaled);
}

}  // namespace nickel::graphics