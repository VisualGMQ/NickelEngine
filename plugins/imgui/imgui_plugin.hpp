#pragma once

#include "nickel.hpp"
#include "video/window.hpp"
#include "video/event.hpp"
#include "SDL.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#ifdef NICKEL_HAS_VULKAN
#include "imgui_impl_vulkan.h"
#include "graphics/rhi/vk/adapter.hpp"
#include "graphics/rhi/vk/device.hpp"
#include "graphics/rhi/vk/queue.hpp"
#include "graphics/rhi/vk/util.hpp"
#endif
#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"



namespace plugin {

using namespace nickel;

void ImGuiInit(gecs::resource<gecs::mut<Window>>,
               gecs::resource<gecs::mut<EventPoller>>,
               gecs::resource<rhi::Adapter>,
               gecs::resource<rhi::Device>);

void ImGuiStart(gecs::resource<rhi::Adapter>);

void ImGuiEnd(gecs::resource<gecs::mut<Window>>, gecs::resource<rhi::Adapter>,
              gecs::resource<rhi::Device>,
              gecs::resource<gecs::mut<RenderContext>>);

void ImGuiShutdown(gecs::resource<rhi::Adapter>, gecs::resource<rhi::Device>);

}  // namespace plugin