#include "common.hpp"

RenderTestCommonContext::RenderTestCommonContext() {
    initSyncObjects();
}

void RenderTestCommonContext::BeginFrame() {
    auto fence = GetFence();

    auto device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
    m_swapchain_image_index = device.WaitAndAcquireSwapchainImageIndex(
        GetImageAvaliableSemaphore(), std::span{&fence, 1});
}

nickel::graphics::Semaphore&
RenderTestCommonContext::GetImageAvaliableSemaphore() {
    return m_image_avaliable_sems[m_render_frame_index];
}

nickel::graphics::Semaphore&
RenderTestCommonContext::GetRenderFinishSemaphore() {
    return m_render_finish_sems[m_render_frame_index];
}

nickel::graphics::Fence& RenderTestCommonContext::GetFence() {
    return m_present_fences[m_render_frame_index];
}

void RenderTestCommonContext::EndFrame() {
    auto& ctx = nickel::Context::GetInst();
    auto device = ctx.GetGPUAdapter().GetDevice();

    device.Present(std::span{&GetRenderFinishSemaphore(), 1});
    device.EndFrame();

    m_render_frame_index = (m_render_frame_index + 1) %
                           device.GetSwapchainImageInfo().m_image_count;
}

uint32_t RenderTestCommonContext::CurFrameIdx() const {
    return m_render_frame_index;
}

void RenderTestCommonContext::initSyncObjects() {
    auto device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();

    uint32_t swapchain_image_count =
        device.GetSwapchainImageInfo().m_image_count;
    for (uint32_t i = 0; i < swapchain_image_count; i++) {
        m_present_fences.push_back(device.CreateFence(true));
        m_image_avaliable_sems.push_back(device.CreateSemaphore());
        m_render_finish_sems.push_back(device.CreateSemaphore());
    }
}

