#include "graphics/rhi/device.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/common.hpp"

namespace nickel::rhi {

Device::Device(AdapterImpl& adapter) {
    switch (adapter.RequestAdapterInfo().api) {
        case APIPreference::Undefine:
            break;
        case APIPreference::GL:
            break;
        case APIPreference::Vulkan: {
            auto& adapterImpl = static_cast<vulkan::AdapterImpl&>(adapter);
            impl_ = new vulkan::DeviceImpl(adapterImpl);
            break;
        }
    }
}

void Device::Destroy() {
    if (impl_) {
        delete impl_;
        impl_ = nullptr;
    }
}


Texture Device::CreateTexture(const Texture::Descriptor& desc) {
    return impl_->CreateTexture(desc);
}

RenderPipeline Device::CreateRenderPipeline(const RenderPipeline::Descriptor& desc) {
    return impl_->CreateRenderPipeline(desc);
}

PipelineLayout Device::CreatePipelineLayout(const PipelineLayout::Descriptor& desc) {
    return impl_->CreatePipelineLayout(desc);
}

Sampler Device::CreateSampler(const Sampler::Descriptor& desc) {
    return impl_->CreateSampler(desc);
}

CommandEncoder Device::CreateCommandEncoder() {
    return impl_->CreateCommandEncoder();
}

BindGroup Device::CreateBindGroup(const BindGroup::Descriptor& desc) {
    return impl_->CreateBindGroup(desc);
}

BindGroupLayout Device::CreateBindGroupLayout(const BindGroupLayout::Descriptor& desc) {
    return impl_->CreateBindGroupLayout(desc);
}

ShaderModule Device::CreateShaderModule(const ShaderModule::Descriptor& desc) {
    return impl_->CreateShaderModule(desc);
}

Buffer Device::CreateBuffer(const Buffer::Descriptor& desc) {
    return impl_->CreateBuffer(desc);
}

void Device::SwapContext() {
    return impl_->SwapContext();
}

void Device::WaitIdle() {
    return impl_->WaitIdle();
}

Queue Device::GetQueue() {
    return impl_->GetQueue();
}

}  // namespace nickel::rhi