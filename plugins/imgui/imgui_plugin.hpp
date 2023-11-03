#pragma once

#include "nickel.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

namespace plugin {

using namespace nickel;

void ImGuiInit(gecs::resource<gecs::mut<Window>> window,
               gecs::resource<gecs::mut<Renderer2D>> renderer2d,
               gecs::resource<gecs::mut<EventPoller>> poller);
void ImGuiStart();

void ImGuiEnd(gecs::resource<gecs::mut<Window>> window,
              gecs::resource<gecs::mut<Renderer2D>> renderer2d);

void ImGuiShutdown();

}