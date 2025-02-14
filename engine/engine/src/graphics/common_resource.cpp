#include "nickel/graphics/common_resource.hpp"

#include "nickel/nickel.hpp"

namespace nickel::graphics {
CommonResource::CommonResource(Device device, const video::Window& window) {
    initDepthImages(device, window);
    initRenderPass(device);
    initFramebuffers(device);
    initSyncObjects(device);
    initCameraBuffer(device);
    initDefaultResources(device);
}

ImageView CommonResource::GetDepthImageView(uint32_t idx) {
    return m_depth_image_views[idx];
}

Framebuffer CommonResource::GetFramebuffer(uint32_t idx) {
    return m_fbos[idx];
}

Semaphore& CommonResource::GetImageAvaliableSemaphore(uint32_t idx) {
    return m_image_avaliable_sems[idx];
}

Semaphore& CommonResource::GetRenderFinishSemaphore(uint32_t idx) {
    return m_render_finish_sems[idx];
}

Semaphore& CommonResource::GetImGuiRenderFinishSemaphore(uint32_t idx) {
    return m_imgui_render_finish_sems[idx];
}

Fence& CommonResource::GetFence(uint32_t idx) {
    return m_present_fences[idx];
}

void CommonResource::Begin() {
    m_camera_buffer.MapAsync();
    auto& camera = nickel::Context::GetInst().GetCamera();
    memcpy(m_camera_buffer.GetMappedRange(), camera.GetProject().Ptr(),
           sizeof(Mat44));

    m_view_buffer.MapAsync();
    Vec3 pos = camera.GetPosition();
    memcpy(m_view_buffer.GetMappedRange(), pos.Ptr(), sizeof(Vec3));
}

void CommonResource::End() {
    m_view_buffer.Unmap();
    m_camera_buffer.Unmap();
}

void CommonResource::initDepthImages(Device& device,
                                     const video::Window& window) {
    for (uint32_t i = 0; i < device.GetSwapchainImageInfo().m_image_count;
         i++) {
        {
            auto size = window.GetSize();
            Image::Descriptor desc;
            desc.imageType = ImageType::Dim2;
            desc.extent.w = size.w;
            desc.extent.h = size.h;
            desc.extent.l = 1;
            desc.format = Format::D32_SFLOAT_S8_UINT;
            desc.usage = ImageUsage::DepthStencilAttachment;
            desc.tiling = ImageTiling::Optimal;
            m_depth_images.emplace_back(device.CreateImage(desc));
        }
        {
            ImageView::Descriptor view_desc;
            view_desc.format = Format::D32_SFLOAT_S8_UINT;
            view_desc.components = ComponentMapping::SwizzleIdentity;
            view_desc.subresourceRange.aspectMask =
                Flags{ImageAspect::Depth} | ImageAspect::Stencil;
            view_desc.viewType = ImageViewType::Dim2;
            m_depth_image_views.emplace_back(
                m_depth_images.back().CreateView(view_desc));
        }
    }
}

void CommonResource::initFramebuffers(Device& device) {
    auto swapchain_image_views = device.GetSwapchainImageViews();
    auto image_extent = device.GetSwapchainImageInfo().m_extent;
    for (uint32_t i = 0; i < device.GetSwapchainImageInfo().m_image_count;
         i++) {
        Framebuffer::Descriptor desc;
        desc.m_extent.w = image_extent.w;
        desc.m_extent.h = image_extent.h;
        desc.m_extent.l = 1;
        desc.m_render_pass = m_render_pass;
        desc.m_views = {swapchain_image_views[i], m_depth_image_views[i]};
        m_fbos.push_back(device.CreateFramebuffer(desc));
    }
}

void CommonResource::initRenderPass(Device& device) {
    RenderPass::Descriptor desc;

    {
        RenderPass::Descriptor::AttachmentDescription attachment;
        attachment.samples = SampleCount::Count1;
        attachment.initialLayout = ImageLayout::Undefined;
        attachment.finalLayout = ImageLayout::ColorAttachmentOptimal;
        attachment.loadOp = AttachmentLoadOp::Clear;
        attachment.storeOp = AttachmentStoreOp::Store;
        attachment.stencilLoadOp = AttachmentLoadOp::DontCare;
        attachment.stencilStoreOp = AttachmentStoreOp::DontCare;
        attachment.format =
            device.GetSwapchainImageInfo().m_surface_format.format;
        desc.attachments.push_back(attachment);
    }

    {
        RenderPass::Descriptor::AttachmentDescription attachment;
        attachment.samples = SampleCount::Count1;
        attachment.initialLayout = ImageLayout::Undefined;
        attachment.finalLayout = ImageLayout::DepthStencilAttachmentOptimal;
        attachment.loadOp = AttachmentLoadOp::Clear;
        attachment.storeOp = AttachmentStoreOp::Store;
        attachment.stencilLoadOp = AttachmentLoadOp::DontCare;
        attachment.stencilStoreOp = AttachmentStoreOp::DontCare;
        attachment.format = Format::D32_SFLOAT_S8_UINT;
        desc.attachments.push_back(attachment);
    }

    RenderPass::Descriptor::SubpassDescription subpass;

    {
        RenderPass::Descriptor::AttachmentReference ref;
        ref.attachment = 0;
        ref.layout = ImageLayout::ColorAttachmentOptimal;
        subpass.colorAttachments.push_back(ref);
    }

    {
        RenderPass::Descriptor::AttachmentReference ref;
        ref.attachment = 1;
        ref.layout = ImageLayout::DepthStencilAttachmentOptimal;
        subpass.depthStencilAttachment = ref;
    }
    desc.subpasses.push_back(subpass);

    RenderPass::Descriptor::SubpassDependency deps;
    deps.srcSubpass =
        RenderPass::Descriptor::SubpassDependency::ExternalSubpass;
    deps.dstSubpass = 0;
    deps.srcAccessMask = Access::None;
    deps.dstAccessMask = Access::ColorAttachmentWrite;
    deps.srcStageMask = PipelineStage::TopOfPipe;
    deps.dstStageMask = PipelineStage::ColorAttachmentOutput;
    desc.dependencies.push_back(deps);

    m_render_pass = device.CreateRenderPass(desc);
}

void CommonResource::initSyncObjects(Device& device) {
    uint32_t swapchain_image_count =
        device.GetSwapchainImageInfo().m_image_count;
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        m_present_fences.push_back(device.CreateFence(true));
        m_image_avaliable_sems.push_back(device.CreateSemaphore());
        m_render_finish_sems.push_back(device.CreateSemaphore());
        m_imgui_render_finish_sems.push_back(device.CreateSemaphore());
    }
}

void CommonResource::initDefaultResources(Device& device) {
    {
        Sampler::Descriptor desc;
        desc.minFilter = Filter::Linear;
        desc.magFilter = Filter::Linear;
        desc.addressModeW = SamplerAddressMode::Repeat;
        desc.addressModeU = SamplerAddressMode::Repeat;
        desc.addressModeV = SamplerAddressMode::Repeat;
        m_default_sampler = device.CreateSampler(desc);
    }

    m_white_image = createPureColorImage(device, 0xFFFFFFFF);
    m_black_image = createPureColorImage(device, 0xFF000000);
    m_default_normal_image = createPureColorImage(device, 0xFFFF8080);
}

void CommonResource::initCameraBuffer(Device& device) {
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::Coherence;
        desc.m_size = sizeof(Mat44);
        desc.m_usage = BufferUsage::Uniform;
        m_camera_buffer = device.CreateBuffer(desc);
    }

    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::Coherence;
        desc.m_size = sizeof(Vec3);
        desc.m_usage = BufferUsage::Uniform;
        m_view_buffer = device.CreateBuffer(desc);
    }
}

ImageView CommonResource::createPureColorImage(Device& device, uint32_t color) {
    Image image;
    {
        Image::Descriptor desc;
        desc.imageType = ImageType::Dim2;
        desc.extent.w = 1;
        desc.extent.h = 1;
        desc.extent.l = 1;
        desc.format = Format::R8G8B8A8_UNORM;
        desc.usage = Flags{ImageUsage::Sampled} | ImageUsage::CopyDst;
        desc.tiling = ImageTiling::Optimal;
        image = device.CreateImage(desc);
    }
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::CPULocal;
        desc.m_size = 4;
        desc.m_usage = BufferUsage::CopySrc;
        Buffer buffer = device.CreateBuffer(desc);
        buffer.MapAsync();
        void* data = buffer.GetMappedRange();
        memcpy(data, &color, desc.m_size);
        buffer.Unmap();

        CommandEncoder encoder = device.CreateCommandEncoder();
        CopyEncoder copy = encoder.BeginCopy();
        CopyEncoder::BufferImageCopy copy_info;
        copy_info.bufferOffset = 0;
        copy_info.imageExtent.w = 1;
        copy_info.imageExtent.h = 1;
        copy_info.imageExtent.l = 1;
        copy_info.bufferImageHeight = 0;
        copy_info.bufferRowLength = 0;
        copy_info.imageSubresource.aspectMask = ImageAspect::Color;
        copy.CopyBufferToTexture(buffer, image, copy_info);
        copy.End();
        Command cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
        device.WaitIdle();
    }
    {
        ImageView::Descriptor view_desc;
        view_desc.format = Format::R8G8B8A8_UNORM;
        view_desc.components = ComponentMapping::SwizzleIdentity;
        view_desc.subresourceRange.aspectMask = Flags{ImageAspect::Color};
        view_desc.viewType = ImageViewType::Dim2;
        return image.CreateView(view_desc);
    }
}

}  // namespace nickel::graphics
