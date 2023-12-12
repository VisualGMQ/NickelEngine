#include "core/cgmath.hpp"
#include "core/utf8string.hpp"
#include "imgui_plugin.hpp"

#include "asset_list_window.hpp"
#include "content_browser.hpp"
#include "file_dialog.hpp"
#include "mirrow/drefl/cast_any.hpp"
#include "mirrow/drefl/factory.hpp"
#include "mirrow/drefl/make_any.hpp"
#include "misc/project.hpp"
#include "refl/drefl.hpp"
#include "show_component.hpp"
#include "spawn_component.hpp"
#include "ui/style.hpp"
#include "asset_property_window.hpp"

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

                projInitInfo->projectPath = dir.string();

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
        ImGui::End();
    }
}

void ShowVec2(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry,
              const std::vector<int>&) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec2>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec2>(value);
    ImGui::DragFloat2(name.data(), vec->data);
}

void ShowVec3(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry,
              const std::vector<int>&) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec3>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec3>(value);
    ImGui::DragFloat3(name.data(), vec->data);
}

void ShowVec4(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry,
              const std::vector<int>& attrs) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec4>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec4>(value);
    if (std::find(attrs.begin(), attrs.end(), AttrRange01) != attrs.end()) {
        ImGui::DragFloat4(name.data(), vec->data, 0.01, 0, 1);
    } else if (std::find(attrs.begin(), attrs.end(), AttrColor) !=
               attrs.end()) {
        ImGui::ColorEdit4(name.data(), vec->data);
    } else {
        ImGui::DragFloat4(name.data(), vec->data);
    }
}

template <typename MgrType>
nickel::Handle<typename MgrType::AssetType> SelectAndChangeAsset(
    MgrType& mgr, std::string_view buttonText, const std::string& title) {
    using AssetType = typename MgrType::AssetType;
    if (ImGui::Button(buttonText.data())) {
        ImGui::OpenPopup(title.c_str());
    }
    if constexpr (std::is_same_v<AssetType, nickel::Texture>) {
        return TextureAssetWindow(mgr, title);
    } else if constexpr (std::is_same_v<AssetType, nickel::Font>) {
        return FontAssetWindow(mgr, title);
    }
    return {};
}

void ShowTextureHandle(const mirrow::drefl::type* type, std::string_view name,
                       mirrow::drefl::any& value, gecs::registry reg,
                       const std::vector<int>&) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<TextureHandle>(),
        "type incorrect");

    auto& handle = *mirrow::drefl::try_cast<TextureHandle>(value);
    auto mgr = reg.res<TextureManager>();
    char buf[1024] = {0};
    if (mgr->Has(handle)) {
        auto& texture = mgr->Get(handle);
        std::filesystem::path texturePath = texture.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", texturePath.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no texture");
    }

    static std::string title = "asset texture";
    auto newHandle = SelectAndChangeAsset(
        reg.res<gecs::mut<TextureManager>>().get(), buf, title);
    if (newHandle) {
        handle = newHandle;
    }

    if (handle && ImGui::TreeNode("thumbnail")) {
        auto& texture = mgr->Get(handle);
        ImGui::Image(texture.Raw(), ImVec2(texture.Width(), texture.Height()));
        ImGui::TreePop();
    }
}

void ShowFontHandle(const mirrow::drefl::type* type, std::string_view name,
                    mirrow::drefl::any& value, gecs::registry reg,
                    const std::vector<int>&) {
    Assert(type->is_class() && type == ::mirrow::drefl::typeinfo<FontHandle>(),
           "type incorrect");

    auto& handle = *mirrow::drefl::try_cast<FontHandle>(value);
    auto mgr = reg.res<FontManager>();
    char buf[1024] = {0};
    if (mgr->Has(handle)) {
        auto& font = mgr->Get(handle);
        std::filesystem::path path = font.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", path.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no font");
    }

    static std::string title = "asset font";
    auto newHandle =
        SelectAndChangeAsset(reg.res<FontManager>().get(), buf, title);
    if (newHandle) {
        handle = newHandle;
    }
}

void ShowAnimationPlayer(const mirrow::drefl::type* type, std::string_view name,
                         mirrow::drefl::any& value, gecs::registry reg,
                         const std::vector<int>&) {
    Assert(type->is_class() &&
               type == ::mirrow::drefl::typeinfo<AnimationPlayer>(),
           "type incorrect");

    AnimationPlayer& player = *mirrow::drefl::try_cast<AnimationPlayer>(value);
    auto handle = player.GetAnim();
    auto& mgr = reg.res<AnimationManager>().get();
    static char buf[1024] = {0};
    if (handle && mgr.Has(handle)) {
        // snprintf(buf, sizeof(buf), "%s", mgr.Get(handle).RelativePath());
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

void ShowLabel(const mirrow::drefl::type* type, std::string_view name,
               mirrow::drefl::any& value, gecs::registry reg,
               const std::vector<int>&) {
    Assert(type->is_class() && type == ::mirrow::drefl::typeinfo<ui::Label>(),
           "type incorrect");

    ui::Label& label = *mirrow::drefl::try_cast<ui::Label>(value);

    static char buf[1024] = {0};
    auto text = label.GetText().to_string();
    std::strcpy(buf, text.c_str());

    ImGui::InputTextMultiline("text", buf, sizeof(buf));

    if (buf != text) {
        utf8string str{buf};
        label.SetText(str);
    }

    ImGui::ColorEdit4("color", label.color.data);
    ImGui::ColorEdit4("pressing color", label.pressColor.data);
    ImGui::ColorEdit4("hovering color", label.hoverColor.data);

    int size = label.GetPtSize();
    ImGui::DragInt("pt", &size, 1.0, 1);
    if (size != label.GetPtSize()) {
        label.SetPtSize(size);
    }

    // show font
    auto fontHandle = label.GetFont();
    auto ref = mirrow::drefl::any_make_ref(fontHandle);
    ShowFontHandle(ref.type_info(), "font", ref, reg, {});
    label.ChangeFont(fontHandle);
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
    instance.Regist(::mirrow::drefl::typeinfo<ui::Label>(), ShowLabel);
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
    instance.Regist<Sprite>(GeneralSpawnMethod<Sprite>);
    instance.Regist<AnimationPlayer>(SpawnAnimationPlayer);
    instance.Regist<ui::Style>(GeneralSpawnMethod<ui::Style>);
    instance.Regist<ui::Button>(GeneralSpawnMethod<ui::Button>);
    instance.Regist<ui::Label>(GeneralSpawnMethod<ui::Label>);
}

void EditorEnter(gecs::resource<gecs::mut<ProjectInitInfo>> initInfo,
                 gecs::resource<gecs::mut<Window>> window,
                 gecs::resource<gecs::mut<TextureManager>> textureMgr,
                 gecs::resource<gecs::mut<FontManager>> fontMgr,
                 gecs::resource<gecs::mut<Renderer2D>> renderer,
                 gecs::commands cmds) {
    std::string path = initInfo->projectPath + "/project.toml";

    cmds.emplace_resource<AssetPropertyWindowContext>();
    auto& editorCtx = cmds.emplace_resource<EditorContext>();

    auto newInfo = LoadBasicProjectConfig(initInfo->projectPath);
    InitProjectByConfig(newInfo, window.get(), textureMgr.get());
    window->SetTitle("NickelEngine Editor - " + newInfo.windowData.title);
    window->Resize(EditorWindowWidth, EditorWindowHeight);

    // init resource manager root path
    auto resourceDir =
        GenResourcePath(std::filesystem::path{newInfo.projectPath});
    textureMgr->SetRootPath(resourceDir);
    fontMgr->SetRootPath(resourceDir);

    // init content browser info
    auto& contentBrowserInfo = cmds.emplace_resource<ContentBrowserInfo>();
    contentBrowserInfo.path = resourceDir;
    contentBrowserInfo.rootPath = resourceDir;
    if (!std::filesystem::exists(contentBrowserInfo.rootPath)) {
        if (!std::filesystem::create_directory(contentBrowserInfo.rootPath)) {
            LOGF(log_tag::Editor, "create resource dir ",
                 contentBrowserInfo.rootPath, " failed!");
        }
        // TODO: force quit editor
    }
    contentBrowserInfo.RescanDir();

    // TODO: change renderer default texture to canvas

    renderer->SetViewport({0, 0},
                          cgmath::Vec2(EditorWindowWidth, EditorWindowHeight));

    initInfo.get() = std::move(newInfo);

    RegistComponentShowMethods();
    RegistSpawnMethods();
}

void TestEnter(gecs::commands cmds,
               gecs::resource<gecs::mut<TextureManager>> textureMgr,
               gecs::resource<gecs::mut<FontManager>> fontMgr) {
    auto entity = cmds.create();
    auto texture =
        textureMgr->Load("role.png", gogl::Sampler::CreateNearestRepeat());

    SpriteBundle bundle;
    bundle.sprite = Sprite::FromTexture(texture);
    cmds.emplace_bundle<SpriteBundle>(entity, std::move(bundle));

    entity = cmds.create();
    bundle.sprite = Sprite::FromTexture(texture);
    cmds.emplace_bundle<SpriteBundle>(entity, std::move(bundle));

    auto font = fontMgr->Load("arial.ttf");
    entity = cmds.create();
    cmds.emplace<ui::Style>(entity);
    cmds.emplace<ui::Label>(entity, font);
}

void EditorEntityListWindow(bool& show, int& selected, gecs::registry reg,
                            gecs::commands cmds) {
    if (ImGui::Begin("Entity List", &show)) {
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
        ImGui::End();
    }
}

void EditorInspectorWindow(bool& show, gecs::entity entity, gecs::registry reg,
                           gecs::commands cmds) {
    if (ImGui::Begin("Inspector", &show)) {
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
                        func(typeInfo, typeInfo->name(), data, reg,
                             typeInfo->attributes());
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

                ImGui::End();
            }
        }

        if (shouldSpawn) {
            it->second(cmds, entity, reg);
            shouldSpawn = false;
        }

        ImGui::End();
    }
}

void EditorImGuiUpdate(gecs::resource<gecs::mut<Renderer2D>> renderer,
                       gecs::resource<gecs::mut<EditorContext>> ctx,
                       gecs::resource<gecs::mut<ContentBrowserInfo>> cbInfo,
                       gecs::registry reg, gecs::commands cmds) {
    // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    static int selected = -1;
    EditorEntityListWindow(ctx->openEntityList, selected, reg, cmds);
    if (selected >= 0) {
        EditorInspectorWindow(ctx->openInspector,
                              static_cast<typename gecs::entity>(
                                  reg.entities().packed()[selected]),
                              reg, cmds);
    } else {
        EditorInspectorWindow(ctx->openInspector, {}, reg, cmds);
    }

    EditorContentBrowser(ctx->openContentBrowser);

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
