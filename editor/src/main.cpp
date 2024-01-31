#include "imgui_plugin.hpp"
#include "misc/transform.hpp"
#include "nickel.hpp"

#include "asset_property_window.hpp"
#include "config.hpp"
#include "content_browser.hpp"
#include "dialog.hpp"
#include "entity_list_window.hpp"
#include "gizmos.hpp"
#include "inspector.hpp"
#include "show_component.hpp"
#include "spawn_component.hpp"
#include "watch_file.hpp"


enum class EditorScene {
    ProjectManager,
    Editor,
};

void ProjectManagerUpdate(
    gecs::commands cmds, gecs::resource<gecs::mut<EditorContext>> editorCtx,
    gecs::resource<nickel::Window> window,
    gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr) {
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
                editorCtx->projectInfo = CreateNewProject(dir, assetMgr.get());

                LOGI(nickel::log_tag::Nickel, "Create new project to ", dir);
                if (!ChDir(editorCtx->projectInfo.projectPath)) {
                    LOGE(nickel::log_tag::Editor, "can't change directory to ",
                         editorCtx->projectInfo.projectPath);
                } else {
                    cmds.switch_state(EditorScene::Editor);
                }
            }
        }

        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Open Project", ImVec2(200, 200))) {
            auto files = OpenFileDialog("Open Project", {".toml"});
            if (!files.empty()) {
                editorCtx->projectInfo.projectPath =
                    std::filesystem::path{files[0]}.parent_path().string();

                if (!ChDir(editorCtx->projectInfo.projectPath)) {
                    LOGE(nickel::log_tag::Editor, "can't change directory to ",
                         editorCtx->projectInfo.projectPath);
                } else {
                    cmds.switch_state(EditorScene::Editor);
                    LOGI(nickel::log_tag::Nickel, "Open project at ",
                         editorCtx->projectInfo.projectPath);
                }
            }
        }
        ImGui::End();
    }
}

void RegistComponentShowMethods() {
    auto& instance = ComponentShowMethods::Instance();

    instance.Regist(::mirrow::drefl::typeinfo<nickel::cgmath::Vec2>(),
                    DisplayVec2);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::cgmath::Vec3>(),
                    DisplayVec3);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::cgmath::Vec4>(),
                    DisplayVec4);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::TextureHandle>(),
                    DisplayTextureHandle);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::Sprite>(), DisplaySprite);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::AnimationPlayer>(),
                    DisplayAnimationPlayer);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::SoundPlayer>(),
                    DisplaySoundPlayer);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::LuaScript>(),
                    DisplayScript);
    instance.Regist(::mirrow::drefl::typeinfo<nickel::ui::Label>(), DisplayLabel);
}

template <typename T>
void GeneralSpawnMethod(gecs::commands cmds, gecs::entity ent,
                        gecs::registry reg) {
    if (reg.template has<T>(ent)) {
        cmds.template replace<T>(ent);
    } else {
        cmds.template emplace<T>(ent);
    }
}

void SpawnAnimationPlayer(gecs::commands cmds, gecs::entity ent,
                          gecs::registry reg) {
    if (reg.template has<nickel::AnimationPlayer>(ent)) {
        cmds.template replace<nickel::AnimationPlayer>(
            ent, reg.res<gecs::mut<nickel::AssetManager>>()->AnimationMgr());
    } else {
        cmds.template emplace<nickel::AnimationPlayer>(
            ent, reg.res<gecs::mut<nickel::AssetManager>>()->AnimationMgr());
    }
}

void RegistSpawnMethods() {
    auto& instance = SpawnComponentMethods::Instance();

    instance.Regist<nickel::Transform>(GeneralSpawnMethod<nickel::Transform>);
    instance.Regist<nickel::GlobalTransform>(
        GeneralSpawnMethod<nickel::GlobalTransform>);
    instance.Regist<nickel::Sprite>(GeneralSpawnMethod<nickel::Sprite>);
    instance.Regist<nickel::AnimationPlayer>(SpawnAnimationPlayer);
    instance.Regist<nickel::ui::Style>(GeneralSpawnMethod<nickel::ui::Style>);
    instance.Regist<nickel::ui::Button>(GeneralSpawnMethod<nickel::ui::Button>);
    instance.Regist<nickel::ui::Label>(GeneralSpawnMethod<nickel::ui::Label>);
    instance.Regist<nickel::SoundPlayer>(
        GeneralSpawnMethod<nickel::SoundPlayer>);
    instance.Regist<nickel::LuaScript>(GeneralSpawnMethod<nickel::LuaScript>);
}

void dropFileEventHandle(const nickel::DropFileEvent& event,
                         gecs::resource<gecs::mut<EditorContext>> ctx) {
    auto& cbBrowser = ctx->contentBrowserWindow;
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

void EditorEnter(gecs::resource<gecs::mut<nickel::Window>> window,
                 gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr,
                 gecs::resource<gecs::mut<nickel::FontManager>> fontMgr,
                 gecs::resource<gecs::mut<nickel::Renderer2D>> renderer,
                 gecs::resource<gecs::mut<EditorContext>> editorCtx,
                 gecs::event_dispatcher<ReleaseAssetEvent> releaseAsetEvent,
                 gecs::event_dispatcher<FileChangeEvent> fileChangeEvent,
                 gecs::event_dispatcher<nickel::DropFileEvent> dropFileEvent,
                 gecs::commands cmds) {
    RegistEventHandler(releaseAsetEvent);

    dropFileEvent.sink().add<dropFileEventHandle>();

    editorCtx->projectInfo =
        nickel::LoadProjectInfoFromFile(editorCtx->projectInfo.projectPath);

    auto editorConfig = editorCtx->projectInfo;
    editorConfig.windowData.size.Set(EditorWindowWidth, EditorWindowHeight);
    editorConfig.windowData.title =
        "NickelEngine Editor - " + editorCtx->projectInfo.windowData.title;
    nickel::InitProjectByConfig(editorConfig, window.get(), assetMgr.get());

    auto assetDir =
        nickel::GenAssetsDefaultStoreDir(editorCtx->projectInfo.projectPath);

    cmds.emplace_resource<FileWatcher>(assetDir, *gWorld->cur_registry());
    RegistFileChangeEventHandler(fileChangeEvent);

    // init content browser info
    auto& contentBrowserWindow = editorCtx->contentBrowserWindow;
    contentBrowserWindow.SetRootPath(assetDir);
    contentBrowserWindow.SetCurPath(assetDir);
    std::error_code err;
    if (!std::filesystem::exists(contentBrowserWindow.RootPath(), err)) {
        FS_LOG_ERR(err, contentBrowserWindow.RootPath(), " not exists");
        if (!std::filesystem::create_directory( contentBrowserWindow.RootPath(), err)) {
            FS_LOG_ERR(err, "create resource dir ", assetDir, " failed");
        }
        MessageBox box{"error", "can't open project", MessageBoxType::Error};
        box.Show();
    }
    contentBrowserWindow.RescanDir();

    renderer->SetViewport(
        {0, 0}, nickel::cgmath::Vec2(EditorWindowWidth, EditorWindowHeight));

    RegistComponentShowMethods();
    RegistSpawnMethods();

    window->SetResizable(true);
}

void EditorMenubar(EditorContext& ctx) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("save")) {
                SaveProjectByConfig(gWorld->res<EditorContext>()->projectInfo,
                                    gWorld->res<nickel::AssetManager>().get());
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            auto addMenuItem = [](const std::string& text, Window& window) {
                bool show = window.IsVisible();
                ImGui::MenuItem(text.c_str(), nullptr, &show);
                window.SetVisible(show);
            };
            addMenuItem("game content window", ctx.gameWindow);
            addMenuItem("inspector", ctx.inspectorWindow);
            addMenuItem("entity list", ctx.entityListWindow);
            addMenuItem("content browser", ctx.contentBrowserWindow);
            ImGui::MenuItem("imgui demo window", nullptr, &ctx.openDemoWindow);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorImGuiUpdate(gecs::resource<gecs::mut<EditorContext>> ctx) {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    EditorMenubar(ctx.get());

    ctx->Update();

    if (ctx->openDemoWindow) {
        ImGui::ShowDemoWindow(&ctx->openDemoWindow);
    }
}

void EditorExit() {
    int btn =
        MessageBox{"quit", "want to save project?", MessageBoxType::Warning}
            .AddButton("yes", MessageBox::ButtonType::ReturnKeyDefault)
            .AddButton("no", MessageBox::ButtonType::EscapeKeyDefault)
            .Show();
    if (btn == 0) {
        SaveProjectByConfig(gWorld->res<EditorContext>()->projectInfo,
                            gWorld->res<nickel::AssetManager>().get());
    }
    gWorld->remove_res<EditorContext>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::ProjectInitInfo initInfo;
    initInfo.windowData.title = "Project Manager";
    initInfo.windowData.size.Set(ProjectMgrWindowWidth, ProjectMgrWindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.add_state(EditorScene::ProjectManager)
        .regist_startup_system<InitEditorContext>()
        .regist_startup_system<plugin::ImGuiInit>()
        .regist_shutdown_system<plugin::ImGuiShutdown>()
        // ProjectManager state
        .regist_update_system_to_state<plugin::ImGuiStart>(
            EditorScene::ProjectManager)
        .regist_update_system_to_state<ProjectManagerUpdate>(
            EditorScene::ProjectManager)
        .regist_update_system_to_state<plugin::ImGuiEnd>(
            EditorScene::ProjectManager)
        // Editor state
        .add_state(EditorScene::Editor)
        .regist_enter_system_to_state<EditorEnter>(EditorScene::Editor)
        .regist_exit_system_to_state<EditorExit>(EditorScene::Editor)
        .regist_update_system_to_state<plugin::ImGuiStart>(EditorScene::Editor)
        .regist_update_system_to_state<EditorImGuiUpdate>(EditorScene::Editor)
        .regist_update_system_to_state<plugin::ImGuiEnd>(EditorScene::Editor)
        // start with
        .start_with_state(EditorScene::ProjectManager);
}
