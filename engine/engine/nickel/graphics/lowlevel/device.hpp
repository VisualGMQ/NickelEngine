#pragma once

#include "nickel/common/dllexport.hpp"
#include "nickel/common/math/smatrix.hpp"
#include "nickel/graphics/lowlevel/cmd_encoder.hpp"
#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/cmd.hpp"
#include "nickel/graphics/lowlevel/fence.hpp"
#include "nickel/graphics/lowlevel/framebuffer.hpp"
#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/image.hpp"
#include "nickel/graphics/lowlevel/pipeline_layout.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/graphics/lowlevel/semaphore.hpp"

namespace nickel::graphics {

class DeviceImpl;
class AdapterImpl;

struct SurfaceFormatKHR {
    Format format;
    ImageColorSpace colorSpace;
};

struct SwapchainImageInfo {
    SVector<uint32_t, 2> m_extent;
    uint32_t m_image_count;
    SurfaceFormatKHR m_surface_format;
};

class NICKEL_API Device {
public:
    explicit Device(DeviceImpl* impl);
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
    CommandEncoder CreateCommandEncoder();
    Semaphore CreateSemaphore();
    Fence CreateFence(bool signaled);
    const SwapchainImageInfo& GetSwapchainImageInfo() const;
    std::vector<ImageView> GetSwapchainImageViews() const;
    uint32_t WaitAndAcquireSwapchainImageIndex();

    void EndFrame();

    void WaitIdle();

    void Submit(Command& cmd);

private:
    DeviceImpl* m_impl{};
};

}  // namespace nickel::graphics