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
    Device(Device&& o) noexcept { swap(o, *this); }
    Device(const Device& o) = default;
    Device& operator=(const Device& o) = default;

    Device& operator=(Device&& o) noexcept {
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    Texture CreateTexture(const Texture::Descriptor& desc);
    RenderPipeline CreateRenderPipeline(const RenderPipeline::Descriptor&);
    PipelineLayout CreatePipelineLayout(const PipelineLayout::Descriptor&);
    Sampler CreateSampler(const Sampler::Descriptor&);
    CommandEncoder CreateCommandEncoder();
    BindGroup CreateBindGroup(const BindGroup::Descriptor&);
    BindGroupLayout CreateBindGroupLayout(const BindGroupLayout::Descriptor&);
    ShaderModule CreateShaderModule(const ShaderModule::Descriptor& desc);
    Buffer CreateBuffer(const Buffer::Descriptor&);

    void SwapContext();
    void WaitIdle();
    Queue GetQueue();

    void Destroy();

    DeviceImpl* Impl() const { return impl_; }

private:
    DeviceImpl* impl_{};

    friend void swap(Device& o1, Device& o2) noexcept {
        using std::swap;

        swap(o1.impl_, o2.impl_);
    }
};

}