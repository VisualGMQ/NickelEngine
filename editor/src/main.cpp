#include "core/cgmath.hpp"
#include "imgui_plugin.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "mirrow/drefl/cast_any.hpp"
#include "mirrow/drefl/factory.hpp"
#include "misc/project.hpp"
#include "nickel.hpp"

#include "file_dialog.hpp"
#include "show_component.hpp"
#include "spawn_component.hpp"

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

void ProjectManagerUpdate(
    gecs::commands cmds,
    gecs::resource<gecs::mut<ProjectInitInfo>> projInitInfo,
    gecs::resource<gecs::mut<TextureManager>> textureMgr) {
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

void ShowVec2(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec2>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec2>(value);
    ImGui::DragFloat2(name.data(), vec->data);
}

void ShowVec3(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry) {
    Assert(type->is_class() &&
               type == ::mirrow::drefl::typeinfo<cgmath::Vec3>(),
           "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec3>(value);
    ImGui::DragFloat3(name.data(), vec->data);
}

void ShowVec4(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry) {
    Assert(type->is_class() &&
               type == ::mirrow::drefl::typeinfo<cgmath::Vec4>(),
           "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec4>(value);
    ImGui::DragFloat4(name.data(), vec->data);
}

void ShowTextureHandle(const mirrow::drefl::type* type, std::string_view name,
                       mirrow::drefl::any& value, gecs::registry reg) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<TextureHandle>(),
        "type incorrect");

    auto& handle = *mirrow::drefl::try_cast<TextureHandle>(value);
    auto mgr = reg.res<TextureManager>();
    char buf[1024] = {0};
    if (handle) {
        auto& texture = mgr->Get(handle);
        std::filesystem::path texturePath = texture.Filename(),
                              rootPath = mgr->GetRootPath();
        snprintf(buf, sizeof(buf), "res::%ls",
                 std::filesystem::relative(texturePath, rootPath).c_str());
    }
    ImGui::InputText("texture", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);

    if (handle && ImGui::TreeNode("thumbnail")) {
        auto& texture = mgr->Get(handle);
        ImGui::Image(texture.Raw(), ImVec2(texture.Width(), texture.Height()));
        ImGui::TreePop();
    }
}

void ShowAnimationPlayer(const mirrow::drefl::type* type, std::string_view name,
                         mirrow::drefl::any& value, gecs::registry reg) {
    Assert(type->is_class() &&
               type == ::mirrow::drefl::typeinfo<AnimationPlayer>(),
           "type incorrect");

    AnimationPlayer& player = *mirrow::drefl::try_cast<AnimationPlayer>(value);
    auto handle = player.GetAnim();
    auto& mgr = reg.res<AnimationManager>().get();
    static char buf[1024] = {0};
    if (handle && mgr.Has(handle)) {
        snprintf(buf, sizeof(buf), "%s", mgr.GetFilename(handle).data());
    } else {
        snprintf(buf, sizeof(buf), "%s", "no animation");
    }

    int selectedItem = -1;
    std::array<const char*, 2> items = {"create new", "load"};
    ImGui::Combo(buf, &selectedItem, items.data(), items.size(),
                 ImGuiComboFlags_NoPreview);
    if (selectedItem == 0) {
        // TODO: create new animation and switch to animation panel
    } else if (selectedItem == 1) {
        // TODO: load from disk
    }
}

void RegistComponentShowMethods() {
    auto& instance = ComponentShowMethods::Instance();

    instance.Regist(::mirrow::drefl::typeinfo<cgmath::Vec2>(), ShowVec2);
    instance.Regist(::mirrow::drefl::typeinfo<cgmath::Vec3>(), ShowVec3);
    instance.Regist(::mirrow::drefl::typeinfo<cgmath::Vec4>(), ShowVec4);
    instance.Regist(::mirrow::drefl::typeinfo<TextureHandle>(),
                    ShowTextureHandle);
    instance.Regist(::mirrow::drefl::typeinfo<AnimationPlayer>(),
                    ShowAnimationPlayer);
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
    if (reg.template has<AnimationPlayer>(ent)) {
        cmds.template replace<AnimationPlayer>(
            ent, reg.res<gecs::mut<AnimationManager>>().get());
    } else {
        cmds.template emplace<AnimationPlayer>(
            ent, reg.res<gecs::mut<AnimationManager>>().get());
    }
}

void RegistSpawnMethods() {
    auto& instance = SpawnComponentMethods::Instance();

    instance.Regist<Transform>(GeneralSpawnMethod<Transform>);
    instance.Regist<SpriteBundle>(GeneralSpawnMethod<SpriteBundle>);
    instance.Regist<AnimationPlayer>(SpawnAnimationPlayer);
}

void EditorEnter(gecs::resource<gecs::mut<ProjectInitInfo>> initInfo,
                 gecs::resource<gecs::mut<Window>> window,
                 gecs::resource<gecs::mut<TextureManager>> textureMgr,
                 gecs::resource<gecs::mut<Renderer2D>> renderer) {
    std::string path = initInfo->projectPath + "/project.toml";

    auto newInfo = LoadBasicProjectConfig(initInfo->projectPath);
    InitProjectByConfig(newInfo, window.get(), textureMgr.get());
    window->SetTitle("NickelEngine Editor - " + newInfo.windowData.title);
    window->Resize(EditorWindowWidth, EditorWindowHeight);
    textureMgr->SetRootPath(initInfo->projectPath);
    // TODO: change renderer default texture to canvas

    renderer->SetViewport({0, 0},
                          cgmath::Vec2(EditorWindowWidth, EditorWindowHeight));

    initInfo.get() = std::move(newInfo);

    RegistComponentShowMethods();
    RegistSpawnMethods();
}

void TestEnter(gecs::commands cmds,
               gecs::resource<gecs::mut<TextureManager>> textureMgr) {
    auto entity = cmds.create();
    cmds.emplace<Transform>(entity, Transform::FromTranslation({100, 200}));
    auto texture = textureMgr->Load("resources/role.png",
                                    gogl::Sampler::CreateNearestRepeat());

    SpriteBundle bundle;
    bundle.image = texture;
    bundle.sprite = Sprite::Default();
    cmds.emplace<SpriteBundle>(entity, std::move(bundle));
}

void EditorEntityListWindow(int& selected, gecs::registry reg,
                            gecs::commands cmds) {
    static bool entityListOpen = true;
    if (ImGui::Begin("Entity List", &entityListOpen)) {
        if (ImGui::Button("add")) {
            cmds.create();
        }
        ImGui::SameLine();
        if (ImGui::Button("delete") && selected >= 0) {
            cmds.destroy(static_cast<typename gecs::registry::entity_type>(
                reg.entities().packed()[selected]));
        }

        auto& entities = reg.entities().packed();
        for (int i = 0; i < reg.entities().size(); i++) {
            static char buf[64] = {0};
            std::snprintf(buf, sizeof(buf), "entity(ID %d)", entities[i]);
            if (ImGui::Selectable(buf, selected == i)) {
                selected = i;
            }
        }
    }
    ImGui::End();
}

void EditorInspectorWindow(gecs::entity entity, gecs::registry reg,
                           gecs::commands cmds) {
    static bool inspectorOpen = true;
    if (ImGui::Begin("Inspector", &inspectorOpen)) {
        auto& types = mirrow::drefl::all_typeinfo();

        int id = 0;
        for (auto [name, typeInfo] : types) {
            if (reg.has(entity, typeInfo)) {
                auto data = reg.get_mut(entity, typeInfo);

                auto& methods = ComponentShowMethods::Instance();
                auto func = methods.Find(typeInfo);

                ImGui::PushID(id);
                if (ImGui::Button("delete")) {
                    cmds.remove(entity, typeInfo);
                    ImGui::PopID();
                    continue;
                }
                ImGui::PopID();
                ImGui::SameLine();
                if (ImGui::CollapsingHeader(typeInfo->name().c_str())) {
                    if (func) {
                        func(typeInfo, typeInfo->name(), data, reg);
                    }
                }
            }
            id++;
        }

        // show add componet button
        static std::vector<const char*> items;
        items.clear();

        auto& spawnMethods = SpawnComponentMethods::Instance();

        for (auto& [type, spawnFn] : spawnMethods.Methods()) {
            items.push_back(type->name().data());
        }

        ImGui::Separator();

        static int selectedItem = -1;

        typename SpawnComponentMethods::spawn_fn spawn = nullptr;

        ImGui::Combo("components", &selectedItem, items.data(), items.size());

        auto it = spawnMethods.Methods().begin();
        int i = 0;

        while (it != spawnMethods.Methods().end() && i != selectedItem) {
            it++;
            i++;
        }

        static bool replaceHintOpen = false;
        static bool shouldSpawn = false;

        if (ImGui::Button("add component") &&
            it != spawnMethods.Methods().end()) {
            if (reg.has(entity, it->first)) {
                replaceHintOpen = true;
            } else {
                shouldSpawn = true;
            }
        }

        if (replaceHintOpen) {
            ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowFocus();
            if (ImGui::Begin("spawn hint", 0)) {
                ImGui::Text(
                    "component already exists, do you want replace it?");
                if (ImGui::Button("yes")) {
                    shouldSpawn = true;
                    replaceHintOpen = false;
                }

                ImGui::SameLine();
                if (ImGui::Button("cancel")) {
                    replaceHintOpen = false;
                }
            }
            ImGui::End();
        }

        if (shouldSpawn) {
            it->second(cmds, entity, reg);
            shouldSpawn = false;
        }
    }
    ImGui::End();
}

void EditorImGuiUpdate(gecs::resource<gecs::mut<Renderer2D>> renderer,
                       gecs::registry reg, gecs::commands cmds) {
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    static int selected = -1;
    EditorEntityListWindow(selected, reg, cmds);
    if (selected >= 0) {
        EditorInspectorWindow(static_cast<typename gecs::entity>(
                                  reg.entities().packed()[selected]),
                              reg, cmds);
    } else {
        EditorInspectorWindow({}, reg, cmds);
    }

    static bool demoWindowOpen = true;

    ImGui::ShowDemoWindow(&demoWindowOpen);
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo initInfo;
    initInfo.windowData.title = "Project Manager";
    initInfo.windowData.size.Set(ProjectMgrWindowWidth, ProjectMgrWindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.commands().emplace_resource<ProjectInitInfo>(std::move(initInfo));

    reg.add_state(EditorScene::ProjectManager)
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
        .regist_enter_system_to_state<TestEnter>(
            EditorScene::Editor)  // for test
        .regist_update_system_to_state<plugin::ImGuiStart>(EditorScene::Editor)
        .regist_update_system_to_state<EditorImGuiUpdate>(EditorScene::Editor)
        .regist_update_system_to_state<plugin::ImGuiEnd>(EditorScene::Editor)
        // start with
        .start_with_state(EditorScene::ProjectManager);
}
