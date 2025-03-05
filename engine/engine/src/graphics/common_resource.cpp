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
            desc.m_image_type = ImageType::Dim2;
            desc.m_extent.w = size.w;
            desc.m_extent.h = size.h;
            desc.m_extent.l = 1;
            desc.m_format = Format::D32_SFLOAT_S8_UINT;
            desc.m_usage = ImageUsage::DepthStencilAttachment;
            desc.m_tiling = ImageTiling::Optimal;
            m_depth_images.emplace_back(device.CreateImage(desc));
        }
        {
            ImageView::Descriptor view_desc;
            view_desc.m_format = Format::D32_SFLOAT_S8_UINT;
            view_desc.m_components = ComponentMapping::SwizzleIdentity;
            view_desc.m_subresource_range.m_aspect_mask =
                Flags{ImageAspect::Depth} | ImageAspect::Stencil;
            view_desc.m_view_type = ImageViewType::Dim2;
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
        attachment.m_samples = SampleCount::Count1;
        attachment.m_initial_layout = ImageLayout::Undefined;
        attachment.m_final_layout = ImageLayout::ColorAttachmentOptimal;
        attachment.m_load_op = AttachmentLoadOp::Clear;
        attachment.m_store_op = AttachmentStoreOp::Store;
        attachment.m_stencil_load_op = AttachmentLoadOp::DontCare;
        attachment.m_stencil_store_op = AttachmentStoreOp::DontCare;
        attachment.m_format =
            device.GetSwapchainImageInfo().m_surface_format.format;
        desc.m_attachments.push_back(attachment);
    }

    {
        RenderPass::Descriptor::AttachmentDescription attachment;
        attachment.m_samples = SampleCount::Count1;
        attachment.m_initial_layout = ImageLayout::Undefined;
        attachment.m_final_layout = ImageLayout::DepthStencilAttachmentOptimal;
        attachment.m_load_op = AttachmentLoadOp::Clear;
        attachment.m_store_op = AttachmentStoreOp::Store;
        attachment.m_stencil_load_op = AttachmentLoadOp::DontCare;
        attachment.m_stencil_store_op = AttachmentStoreOp::DontCare;
        attachment.m_format = Format::D32_SFLOAT_S8_UINT;
        desc.m_attachments.push_back(attachment);
    }

    RenderPass::Descriptor::SubpassDescription subpass;

    {
        RenderPass::Descriptor::AttachmentReference ref;
        ref.m_attachment = 0;
        ref.m_layout = ImageLayout::ColorAttachmentOptimal;
        subpass.m_color_attachments.push_back(ref);
    }

    {
        RenderPass::Descriptor::AttachmentReference ref;
        ref.m_attachment = 1;
        ref.m_layout = ImageLayout::DepthStencilAttachmentOptimal;
        subpass.m_depth_stencil_attachment = ref;
    }
    desc.m_subpasses.push_back(subpass);

    RenderPass::Descriptor::SubpassDependency deps;
    deps.m_src_subpass =
        RenderPass::Descriptor::SubpassDependency::ExternalSubpass;
    deps.m_dst_subpass = 0;
    deps.m_src_access_mask = Access::None;
    deps.m_dst_access_mask = Access::ColorAttachmentWrite;
    deps.m_src_stage_mask = PipelineStage::TopOfPipe;
    deps.m_dst_stage_mask = PipelineStage::ColorAttachmentOutput;
    desc.m_dependencies.push_back(deps);

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
        desc.m_min_filter = Filter::Linear;
        desc.m_mag_filter = Filter::Linear;
        desc.m_address_mode_w = SamplerAddressMode::Repeat;
        desc.m_address_mode_u = SamplerAddressMode::Repeat;
        desc.m_address_mode_v = SamplerAddressMode::Repeat;
        m_default_sampler = device.CreateSampler(desc);
    }

    m_white_image = createPureColorImage(device, 0xFFFFFFFF);
    m_default_image = m_white_image;
    m_black_image = createPureColorImage(device, 0xFF000000);
    m_default_normal_image = createPureColorImage(device, 0xFFFF8080);
}

void CommonResource::initCameraBuffer(Device& device) {
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::Coherence;
        desc.m_size = sizeof(Mat44);
        desc.m_usage = BufferUsage::Uniform;
        Buffer buffer = device.CreateBuffer(desc);
        m_camera_buffer = device.CreateBuffer(desc);
        Buffer buffer2 = device.CreateBuffer(desc);
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
        desc.m_image_type = ImageType::Dim2;
        desc.m_extent.w = 1;
        desc.m_extent.h = 1;
        desc.m_extent.l = 1;
        desc.m_format = Format::R8G8B8A8_UNORM;
        desc.m_usage = Flags{ImageUsage::Sampled} | ImageUsage::CopyDst;
        desc.m_tiling = ImageTiling::Optimal;
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
        copy_info.m_buffer_offset = 0;
        copy_info.m_image_extent.w = 1;
        copy_info.m_image_extent.h = 1;
        copy_info.m_image_extent.l = 1;
        copy_info.m_buffer_image_height = 0;
        copy_info.m_buffer_row_length = 0;
        copy_info.m_image_subresource.m_aspect_mask = ImageAspect::Color;
        copy.CopyBufferToTexture(buffer, image, copy_info);
        copy.End();
        Command cmd = encoder.Finish();
        device.Submit(cmd, {}, {}, {});
        device.WaitIdle();
    }
    {
        ImageView::Descriptor view_desc;
        view_desc.m_format = Format::R8G8B8A8_UNORM;
        view_desc.m_components = ComponentMapping::SwizzleIdentity;
        view_desc.m_subresource_range.m_aspect_mask = Flags{ImageAspect::Color};
        view_desc.m_view_type = ImageViewType::Dim2;
        return image.CreateView(view_desc);
    }
}

}  // namespace nickel::graphics
