﻿#include "nickel/graphics/common_resource.hpp"

namespace nickel::graphics {
CommonResource::CommonResource(Device device, const video::Window& window) {
    initDepthImages(device, window);
    initRenderPass(device);
    initFramebuffers(device);
    initSyncObjects(device);
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

}  // namespace nickel::graphics
