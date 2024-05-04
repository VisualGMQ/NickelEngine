#include "common/transform.hpp"
#include "imgui_plugin.hpp"
#include "nickel.hpp"

#include "asset_property_window.hpp"
#include "config.hpp"
#include "content_browser.hpp"
#include "dialog.hpp"
#include "entity_list_window.hpp"
#include "inspector.hpp"
#include "spawn_component.hpp"
#include "type_displayer.hpp"
#include "watch_file.hpp"


enum class EditorScene {
    ProjectManager,
    Editor,
};

void ProjectManagerUpdate(
    gecs::commands cmds, gecs::resource<nickel::Window> window,
    gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr) {
    PROFILE_BEGIN();

    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    auto& editorCtx = EditorContext::Instance();

    if (ImGui::Begin("ProjectManager", nullptr, flags)) {
        if (ImGui::Button("Create New Project", ImVec2(200, 200))) {
            auto dir = OpenDirDialog("Choose Empty Directory");

            if (!dir.empty()) {
                editorCtx.projectInfo = CreateNewProject(dir, assetMgr.get());

                LOGI(nickel::log_tag::Nickel, "Create new project to ", dir);
                if (!ChDir(editorCtx.projectInfo.projectPath)) {
                    LOGE(nickel::log_tag::Editor, "can't change directory to ",
                         editorCtx.projectInfo.projectPath);
                } else {
                    cmds.switch_state(EditorScene::Editor);
                }
            }
        }

        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Open Project", ImVec2(200, 200))) {
            auto files = OpenFileDialog("Open Project", {".toml"});
            if (!files.empty()) {
                editorCtx.projectInfo.projectPath =
                    std::filesystem::path{files[0]}.parent_path().string();

                if (!ChDir(editorCtx.projectInfo.projectPath)) {
                    LOGE(nickel::log_tag::Editor, "can't change directory to ",
                         editorCtx.projectInfo.projectPath);
                } else {
                    cmds.switch_state(EditorScene::Editor);
                    LOGI(nickel::log_tag::Nickel, "Open project at ",
                         editorCtx.projectInfo.projectPath);
                }
            }
        }
        ImGui::End();
    }

    PROFILE_END();
}

void dropFileEventHandle(const nickel::DropFileEvent& event) {
    auto& cbBrowser = EditorContext::Instance().contentBrowserWindow;
    std::filesystem::directory_entry entry(event.path);
    if (!entry.exists()) {
        return;
    }

    auto path = entry.path();
    auto name = path.filename();
    auto newPath = cbBrowser.CurPath() / name;
    std::error_code err;
    FS_CALL(std::filesystem::copy(path, newPath, err), err);
}

void EditorEnter(
    gecs::resource<nickel::rhi::Adapter> adapter,
    gecs::resource<gecs::mut<nickel::Window>> window,
    gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr,
    gecs::resource<gecs::mut<nickel::FontManager>> fontMgr,
    gecs::resource<gecs::mut<nickel::Camera>> camera,
    gecs::event_dispatcher<ReleaseAssetEvent> releaseAsetEvent,
    gecs::event_dispatcher<FileChangeEvent> fileChangeEvent,
    gecs::event_dispatcher<nickel::DropFileEvent> dropFileEvent,
    gecs::event_dispatcher<nickel::WindowResizeEvent> windowResizeEvent,
    gecs::commands cmds) {
    RegistEventHandler(releaseAsetEvent);

    auto& target = EditorContext::Instance().texture;
    auto halfTextureSize = target->Size() * 0.5f;
    camera->SetProject(nickel::cgmath::CreateOrtho(
        -halfTextureSize.w, halfTextureSize.w, halfTextureSize.h,
        -halfTextureSize.h, 1000, -1000,
        adapter->RequestAdapterInfo().api == nickel::rhi::APIPreference::GL));
    camera->SetViewport({
        {0, 0},
        target->Size()
    });

    dropFileEvent.sink().add<dropFileEventHandle>();

    auto& editorCtx = EditorContext::Instance();
    editorCtx.projectInfo =
        nickel::LoadProjectInfoFromFile(editorCtx.projectInfo.projectPath);

    auto editorConfig = editorCtx.projectInfo;
    editorConfig.windowData.size.Set(EditorWindowWidth, EditorWindowHeight);
    editorConfig.windowData.title =
        "NickelEngine Editor - " + editorCtx.projectInfo.windowData.title;
    nickel::InitProjectByConfig(editorConfig, window.get(), assetMgr.get());

    windowResizeEvent.happend(nickel::WindowResizeEvent{
        {EditorWindowWidth, EditorWindowHeight}
    });

    auto assetDir =
        nickel::GenAssetsDefaultStoreDir(editorCtx.projectInfo.projectPath);

    cmds.emplace_resource<FileWatcher>(
        assetDir, *nickel::ECS::Instance().World().cur_registry());
    RegistFileChangeEventHandler(fileChangeEvent);

    // init content browser info
    auto& contentBrowserWindow = editorCtx.contentBrowserWindow;
    contentBrowserWindow.SetRootPath(assetDir);
    contentBrowserWindow.SetCurPath(assetDir);
    std::error_code err;
    if (!std::filesystem::exists(contentBrowserWindow.RootPath(), err)) {
        FS_LOG_ERR(err, contentBrowserWindow.RootPath(), " not exists");
        if (!std::filesystem::create_directory(contentBrowserWindow.RootPath(),
                                               err)) {
            FS_LOG_ERR(err, "create resource dir ", assetDir, " failed");
        }
        MessageBox box{"error", "can't open project", MessageBoxType::Error};
        box.Show();
    }
    contentBrowserWindow.RescanDir();

    RegistDisplayMethods();
    RegistSpawnMethods();

    window->SetResizable(true);
}

void DrawGuizmos() {
    auto reg = nickel::ECS::Instance().World().cur_registry();
    auto camera = reg->res_mut<nickel::Camera>();
    auto mouse = reg->res<nickel::Mouse>();

    // if (IsFocused()) {
    //     if (mouse->MiddleBtn().IsPressing()) {
    //         auto offset = mouse->Offset();
    //         auto scale = camera->Scale();
    //         camera->Move(nickel::cgmath::Vec2{-offset.x, offset.y} / scale);
    //     }

    //     if (auto wheel = mouse->WheelOffset(); wheel.y != 0) {
    //         auto scale = camera->Scale();
    //         auto newScale = scale + wheel.y * nickel::cgmath::Vec2{0.01,
    //         0.01}; if (newScale.x <= 0 || newScale.y <= 0) {
    //             newScale.Set(0.01, 0.01);
    //         }
    //         camera->ScaleTo(newScale);
    //     }
    // }

    auto selectedEnt = EditorContext::Instance().entityListWindow.GetSelected();
    if (!reg->alive(selectedEnt) || !reg->has<nickel::Transform>(selectedEnt)) {
        return;
    }

    static auto guizmoOperation_ = ImGuizmo::OPERATION::TRANSLATE;
    static auto guizmoMode_ = ImGuizmo::MODE::LOCAL;

    auto& transform = reg->get_mut<nickel::Transform>(selectedEnt);

    auto keyboard = reg->res<nickel::Keyboard>();
    if (keyboard->Key(nickel::Key::Q).IsPressed()) {
        guizmoOperation_ = ImGuizmo::TRANSLATE;
    }
    if (keyboard->Key(nickel::Key::W).IsPressed()) {
        guizmoOperation_ = ImGuizmo::SCALE;
    }
    if (keyboard->Key(nickel::Key::E).IsPressed()) {
        guizmoOperation_ = ImGuizmo::ROTATE;
    }
    if (ImGui::RadioButton("Translate",
                           guizmoOperation_ == ImGuizmo::TRANSLATE))
        guizmoOperation_ = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", guizmoOperation_ == ImGuizmo::ROTATE))
        guizmoOperation_ = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", guizmoOperation_ == ImGuizmo::SCALE))
        guizmoOperation_ = ImGuizmo::SCALE;

    nickel::cgmath::Mat44 matrix;
    auto windowSize = reg->res<nickel::Window>()->Size();
    float matrixRot[3] = {0, 0, transform.rotation};
    float matrixTrans[3] = {transform.translation.x, transform.translation.y,
                            0};
    float matrixScale[3] = {transform.scale.x, transform.scale.y, 0};
    ImGuizmo::RecomposeMatrixFromComponents(matrixTrans, matrixRot, matrixScale,
                                            matrix.data);

    if (guizmoOperation_ != ImGuizmo::SCALE) {
        if (ImGui::RadioButton("Local", guizmoMode_ == ImGuizmo::LOCAL))
            guizmoMode_ = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", guizmoMode_ == ImGuizmo::WORLD))
            guizmoMode_ = ImGuizmo::WORLD;
    }
    ImGuizmo::Enable(true);
    ImGuiIO& io = ImGui::GetIO();
    auto pos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
    auto size =
        ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();
    ImGuizmo::SetRect(0, 0, 1024, 720);
    auto view = nickel::cgmath::CreateScale({1, -1, 1}) * camera->View();
    // ImGuizmo::DrawGrid(view.data, camera->Project().data, matrix.data,
    // 100.f);
    ImGuizmo::Manipulate(view.data, camera->Project().data, guizmoOperation_,
                         guizmoMode_, matrix.data, nullptr, nullptr);
}

void EditorImGuiUpdate() {
    PROFILE_BEGIN();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    auto& ctx = EditorContext::Instance();

    ctx.Update();

    if (ctx.openDemoWindow) {
        ImGui::ShowDemoWindow(&ctx.openDemoWindow);
    }

    // DrawGuizmos();
}

void EditorExit(gecs::registry reg) {
    if (reg.cur_state<EditorScene>() == EditorScene::Editor) {
        int btn =
            MessageBox{"quit", "want to save project?", MessageBoxType::Warning}
                .AddButton("yes", MessageBox::ButtonType::ReturnKeyDefault)
                .AddButton("no", MessageBox::ButtonType::EscapeKeyDefault)
                .Show();
        if (btn == 0) {
            SaveProjectByConfig(EditorContext::Instance().projectInfo,
                                nickel::ECS::Instance()
                                    .World()
                                    .res<nickel::AssetManager>()
                                    .get());
        }
    }
    EditorContext::Delete();
}

void RegistSystems(gecs::world& world) {
    auto& reg = *world.cur_registry();
    nickel::RegistEngineSystem(reg);

    reg.regist_startup_system<plugin::ImGuiInit>()
        .regist_shutdown_system_before<plugin::ImGuiShutdown,
                                       nickel::EngineShutdown>()
        .regist_update_system_after<plugin::ImGuiStart, nickel::EndRender>()
        .regist_update_system_after<plugin::ImGuiEnd, plugin::ImGuiStart>()
        .regist_shutdown_system_before<EditorExit, nickel::EngineShutdown>()
        .add_state(EditorScene::ProjectManager)
        .regist_enter_system_to_state<InitEditorContext>(
            EditorScene::ProjectManager)
        .regist_update_system_to_state_after<ProjectManagerUpdate,
                                             plugin::ImGuiStart>(
            EditorScene::ProjectManager)
        .add_state(EditorScene::Editor)
        .regist_enter_system_to_state<EditorEnter>(EditorScene::Editor)
        .regist_update_system_to_state_after<EditorImGuiUpdate,
                                             plugin::ImGuiStart>(
            EditorScene::Editor)
        .regist_update_system_to_state_after<
            plugin::ImGuiGameWindowLayoutTransition, EditorImGuiUpdate>(
            EditorScene::Editor);

    world.cur_registry()->switch_state_immediatly(EditorScene::ProjectManager);
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::ProjectInitInfo initInfo;
    initInfo.windowData.title = "Project Manager";
    initInfo.windowData.size.Set(ProjectMgrWindowWidth, ProjectMgrWindowHeight);
    InitSystem(world, initInfo, reg.commands());
    RegistSystems(world);
}
