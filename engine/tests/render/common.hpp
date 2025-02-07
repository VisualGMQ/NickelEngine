#pragma once
#include "nickel/nickel.hpp"

class RenderTestCommonContext
    : public nickel::Singlton<RenderTestCommonContext, true> {
public:
    RenderTestCommonContext();

    nickel::graphics::Semaphore& GetImageAvaliableSemaphore();
    nickel::graphics::Semaphore& GetRenderFinishSemaphore();
    nickel::graphics::Fence& GetFence();

    void BeginFrame();
    void EndFrame();
    uint32_t CurFrameIdx() const;

private:
    uint32_t m_swapchain_image_index{};
    uint32_t m_render_frame_index{};

    std::vector<nickel::graphics::Fence> m_present_fences;
    std::vector<nickel::graphics::Semaphore> m_image_avaliable_sems;
    std::vector<nickel::graphics::Semaphore> m_render_finish_sems;

    void initSyncObjects();
};
