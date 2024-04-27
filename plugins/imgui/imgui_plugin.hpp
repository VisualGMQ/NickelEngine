#pragma once

#include "SDL.h"
#include "nickel.hpp"
#include "video/event.hpp"
#include "video/window.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "imgui_internal.h"

#ifdef NICKEL_HAS_VULKAN
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "imgui_impl_vulkan.h"

#endif
#include "GraphEditor.h"
#include "ImCurveEdit.h"
#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"

namespace plugin {

using namespace nickel;

#ifdef NICKEL_HAS_VULKAN
#define MAX_DESCRIPTOR_SET_FOR_TEXTURE 100

struct ImGuiVkContext {
    vk::DescriptorPool descriptorPool;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::RenderPass renderPass;
    vk::Sampler combinedSampler;
    std::vector<vk::Framebuffer> framebuffers;

    std::vector<vk::DescriptorSet> descriptorSets;
    size_t curDescriptorSetIndex = 0;

    ImGuiVkContext(rhi::Device device);
    ~ImGuiVkContext();

    vk::DescriptorSet GetTextureBindedDescriptorSet(const Texture&);

    void RecreateFramebuffers(rhi::vulkan::DeviceImpl&);

private:
    rhi::Device device_;

    void initDescriptorSets(vk::Device);
    void initDescriptorPool(vk::Device);
    void initRenderPass(rhi::vulkan::DeviceImpl&);
    void initFramebuffers(rhi::vulkan::DeviceImpl&);
    void initSetLayout(vk::Device);
    void initCombinedSampler(vk::Device);
};
#endif

void ImGuiInit(gecs::commands, gecs::resource<gecs::mut<Window>>,
               gecs::resource<gecs::mut<EventPoller>>,
               gecs::resource<rhi::Adapter>, gecs::resource<rhi::Device>,
               gecs::event_dispatcher<WindowResizeEvent>);

void ImGuiGameWindowLayoutTransition(
    gecs::resource<gecs::mut<rhi::Device>> device,
    gecs::resource<gecs::mut<Camera>>,
    gecs::resource<gecs::mut<RenderContext>>);

void ImGuiStart(gecs::resource<rhi::Adapter>);

void ImGuiEnd(gecs::resource<gecs::mut<Window>>, gecs::resource<rhi::Adapter>,
              gecs::resource<rhi::Device>,
              gecs::resource<gecs::mut<ImGuiVkContext>>,
              gecs::resource<gecs::mut<RenderContext>>);

void ImGuiShutdown(gecs::commands, gecs::resource<rhi::Adapter>,
                   gecs::resource<rhi::Device>);

}  // namespace plugin

namespace ImGui {

void Image(const ::nickel::Texture& texture, const ImVec2& image_size,
           const ImVec2& uv0 = {0, 0}, const ImVec2& uv1 = {1, 1},
           const ImVec4& tint_col = {1, 1, 1, 1},
           const ImVec4& border_col = {0, 0, 0, 0});

bool ImageButton(const char* str_id, const ::nickel::Texture& texture,
                 const ImVec2& image_size, const ImVec2& uv0 = {0, 0},
                 const ImVec2& uv1 = {1, 1},
                 const ImVec4& bg_col = {0, 0, 0, 0},
                 const ImVec4& tint_col = {1, 1, 1, 1});

bool ImageButton(const ::nickel::Texture& texture, const ImVec2& size,
                 const ImVec2& uv0 = {0, 0}, const ImVec2& uv1 = {1, 1},
                 int frame_padding = -1, const ImVec4& bg_col = {0, 0, 0, 0},
                 const ImVec4& tint_col = {1, 1, 1, 1});
}  // namespace ImGui