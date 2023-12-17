#include "imgui_plugin.hpp"

namespace plugin {

using namespace nickel;

void imguiProcessEvent(const SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void ImGuiInit(gecs::resource<gecs::mut<Window>> window,
               gecs::resource<gecs::mut<Renderer2D>> renderer2d,
               gecs::resource<gecs::mut<EventPoller>> poller) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // enable Docking

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)window->Raw(), nullptr);
    ImGui_ImplOpenGL3_Init("#version 430");

    renderer2d->SetClearColor({0.1f, 0.1f, 0.1f, 1.0});

    poller->InjectHandler<imguiProcessEvent>();
}

void ImGuiStart() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void ImGuiEnd(gecs::resource<gecs::mut<Window>> window,
              gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    ImGui::Render();
    auto display_size = window->Size();
    renderer2d->SetViewport({0, 0}, cgmath::Vec2(display_size.w, display_size.h));
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiShutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}


}