#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "nickel.hpp"

#include "file_dialog.hpp"

#include "refl/cgmath.hpp"
#include "refl/sprite.hpp"
#include "refl/tilesheet.hpp"

enum class EditorScene {
    ProjectManager,
    Editor,
};

using namespace nickel;

constexpr int WindowWidth = 1024;
constexpr int WindowHeight = 720;

void ImGuiInit(gecs::resource<gecs::mut<Window>> window,
                gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window->Raw(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    renderer2d->SetClearColor({0.1f, 0.1f, 0.1f, 1.0});
}

void InitEditor(gecs::resource<gecs::mut<Window>> window,
                gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    window->Resize(WindowWidth, WindowHeight);
    window->SetTitle("Nickel Engine Editor");
}

void ImGuiStart() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool show_demo_window = true;
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
}

void EditorShutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiEnd(gecs::resource<gecs::mut<Window>> window,
              gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize((GLFWwindow*)window->Raw(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    renderer2d->Clear();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorDraw() {
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoSavedSettings;

    // We demonstrate using the full viewport area or the work area (without
    // menu-bars, task-bars etc.) Based on your use case you may want one or the
    // other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
                             
    if (ImGui::Begin("Example: Fullscreen window", nullptr, flags)) {
        if (ImGui::Button("Create New Project")) {
            auto dir = OpenDirDialog("Choose Empty Directory");
        }

        if (ImGui::Button("Open Project")) {
            auto files = OpenFileDialog("Open Project");
            if (!files.empty()) {
                // TODO: open project
            }
        }
    }
    ImGui::End();

    // ImGui::ShowDemoWindow();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    reg.add_state(EditorScene::ProjectManager)
        .regist_startup_system<ImGuiInit>()
        .regist_update_system_to_state<ImGuiStart>(EditorScene::ProjectManager)
        .regist_enter_system_to_state<InitEditor>(EditorScene::ProjectManager)
        .regist_update_system_to_state<EditorDraw>(EditorScene::ProjectManager)
        .regist_update_system_to_state<ImGuiEnd>(EditorScene::ProjectManager)
        .regist_shutdown_system<EditorShutdown>()
        .start_with_state(EditorScene::ProjectManager);
}
