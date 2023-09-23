#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "misc/project.hpp"
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

constexpr int EditorWindowWidth = 1024;
constexpr int EditorWindowHeight = 720;

constexpr int ProjectMgrWindowWidth = 450;
constexpr int ProjectMgrWindowHeight = 300;

void ImGuiInit(gecs::resource<gecs::mut<Window>> window,
               gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // enable Docking

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window->Raw(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    renderer2d->SetClearColor({0.1f, 0.1f, 0.1f, 1.0});
}

void ImGuiStart() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
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

void ProjectManagerUpdate(
    gecs::commands cmds,
    gecs::resource<gecs::mut<ProjectInitInfo>> projInitInfo,
    gecs::resource<gecs::mut<TextureManager>> textureMgr) {
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    if (ImGui::Begin("ProjectManager", nullptr, flags)) {
        if (ImGui::Button("Create New Project", ImVec2(200, 200))) {
            auto dir = OpenDirDialog("Choose Empty Directory");

            if (!dir.empty()) {
                ProjectInitInfo initInfo;
                SaveBasicProjectInfo(dir, initInfo);
                textureMgr->ReleaseAll();
                SaveAssets(dir, textureMgr.get());

                projInitInfo->projectPath = dir;

                LOGI(log_tag::Nickel, "Create new project to ", dir);
                cmds.switch_state(EditorScene::Editor);
            }
        }

        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Open Project", ImVec2(200, 200))) {
            auto files = OpenFileDialog("Open Project");
            if (!files.empty()) {
                projInitInfo->projectPath =
                    std::filesystem::path{files[0]}.parent_path().string();
                LOGI(log_tag::Nickel, "Open project at ",
                     projInitInfo->projectPath);

                cmds.switch_state(EditorScene::Editor);
            }
        }
    }
    ImGui::End();
}

void EditorEnter(gecs::resource<ProjectInitInfo> info,
                 gecs::resource<gecs::mut<ProjectInitInfo>> initInfo,
                 gecs::resource<gecs::mut<Window>> window,
                 gecs::resource<gecs::mut<TextureManager>> textureMgr) {
    std::string path = info->projectPath + "/project.toml";

    auto newInfo = LoadBasicProjectConfig(initInfo->projectPath);
    InitProjectByConfig(newInfo, window.get(), textureMgr.get());
    window->SetTitle("NickelEngine Editor - " + newInfo.windowData.title);
    window->Resize(EditorWindowWidth, EditorWindowHeight);
    // TODO: change renderer default texture to canvas

    initInfo.get() = std::move(newInfo);
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo initInfo;
    initInfo.windowData.title = "Project Manager";
    initInfo.windowData.size.Set(ProjectMgrWindowWidth, ProjectMgrWindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.commands().emplace_resource<ProjectInitInfo>(std::move(initInfo));

    reg.add_state(EditorScene::ProjectManager)
        .regist_startup_system<ImGuiInit>()
        .regist_shutdown_system<EditorShutdown>()
        // ProjectManager state
        .regist_update_system_to_state<ImGuiStart>(EditorScene::ProjectManager)
        .regist_update_system_to_state<ProjectManagerUpdate>(
            EditorScene::ProjectManager)
        .regist_update_system_to_state<ImGuiEnd>(EditorScene::ProjectManager)
        // Editor state
        .add_state(EditorScene::Editor)
        .regist_enter_system_to_state<EditorEnter>(EditorScene::Editor)
        // start with
        .start_with_state(EditorScene::ProjectManager);
}
