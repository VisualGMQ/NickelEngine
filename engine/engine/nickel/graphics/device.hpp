#pragma once

#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/bind_group_layout.hpp"
#include "nickel/graphics/buffer.hpp"
#include "nickel/graphics/fence.hpp"
#include "nickel/graphics/framebuffer.hpp"
#include "nickel/graphics/graphics_pipeline.hpp"
#include "nickel/graphics/image.hpp"
#include "nickel/graphics/pipeline_layout.hpp"
#include "nickel/graphics/sampler.hpp"
#include "nickel/graphics/semaphore.hpp"
#include "nickel/video/window.hpp"

namespace nickel::graphics {

class DeviceImpl;
class AdapterImpl;

class Device {
public:
    explicit Device(const AdapterImpl& adapter_impl,
                    const SVector<uint32_t, 2>& window_size);
    ~Device();

    DeviceImpl& Impl();
    const DeviceImpl& Impl() const;

    Buffer CreateBuffer(const Buffer::Descriptor&);
    Image CreateImage(const Image::Descriptor&);
    BindGroupLayout CreateBindGroupLayout(const BindGroupLayout::Descriptor&);
    PipelineLayout CreatePipelineLayout(const PipelineLayout::Descriptor&);
    Framebuffer CreateFramebuffer(const Framebuffer::Descriptor&);
    RenderPass CreateRenderPass(const RenderPass::Descriptor&);
    GraphicsPipeline CreateGraphicPipeline(const GraphicsPipeline::Descriptor&);
    Sampler CreateSampler(const Sampler::Descriptor&);
    ShaderModule CreateShaderModule(const uint32_t* data, size_t size);
    Semaphore CreateSemaphore();
    Fence CreateFence(bool signaled);
    
    void AcquireSwapchainImageAndWait(video::Window& window);

private:
    std::unique_ptr<DeviceImpl> m_impl;
};

}  // namespace nickel::graphics