#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "core/log_tag.hpp"
#include "core/utf8string.hpp"
#include "gecs/entity/entity.hpp"
#include "gecs/entity/querier.hpp"
#include "imgui.h"
#include "imgui_plugin.hpp"

#include "asset_list_window.hpp"
#include "asset_property_window.hpp"
#include "config.hpp"
#include "content_browser.hpp"
#include "file_dialog.hpp"
#include "mirrow/drefl/cast_any.hpp"
#include "mirrow/drefl/factory.hpp"
#include "mirrow/drefl/make_any.hpp"
#include "misc/project.hpp"
#include "refl/drefl.hpp"
#include "renderer/renderer2d.hpp"
#include "show_component.hpp"
#include "spawn_component.hpp"
#include "ui/style.hpp"

enum class EditorScene {
    ProjectManager,
    Editor,
};

using namespace nickel;

void ProjectManagerUpdate(gecs::commands cmds,
                          gecs::resource<gecs::mut<EditorContext>> editorCtx,
                          gecs::resource<Window> window,
                          gecs::resource<gecs::mut<AssetManager>> assetMgr) {
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

                LOGI(log_tag::Nickel, "Create new project to ", dir);
                cmds.switch_state(EditorScene::Editor);
            }
        }

        ImGui::SameLine(0.0f, 20.0f);
        if (ImGui::Button("Open Project", ImVec2(200, 200))) {
            auto files = OpenFileDialog("Open Project");
            if (!files.empty()) {
                editorCtx->projectInfo.projectPath =
                    std::filesystem::path{files[0]}.parent_path().string();
                LOGI(log_tag::Nickel, "Open project at ",
                     editorCtx->projectInfo.projectPath);

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
    auto mgr = reg.res<AssetManager>();
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
        reg.res_mut<AssetManager>()->TextureMgr(), buf, title);
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
    auto& mgr = reg.res<AssetManager>()->FontMgr();
    char buf[1024] = {0};
    if (mgr.Has(handle)) {
        auto& font = mgr.Get(handle);
        std::filesystem::path path = font.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", path.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no font");
    }

    static std::string title = "asset font";
    auto newHandle = SelectAndChangeAsset(mgr, buf, title);
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
    instance.Regist(::mirrow::drefl::typeinfo<FontHandle>(), ShowFontHandle);
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

void createAndSetRenderTarget(nickel::gogl::Framebuffer& fbo, Camera& camera) {
    camera.SetRenderTarget(fbo);
}

void EditorEnter(gecs::resource<gecs::mut<Window>> window,
                 gecs::resource<gecs::mut<AssetManager>> assetMgr,
                 gecs::resource<gecs::mut<FontManager>> fontMgr,
                 gecs::resource<gecs::mut<Renderer2D>> renderer,
                 gecs::resource<gecs::mut<EditorContext>> editorCtx,
                 gecs::resource<gecs::mut<Camera>> camera,
                 gecs::resource<gecs::mut<ui::Context>> uiCtx,
                 gecs::commands cmds) {
    createAndSetRenderTarget(*editorCtx->gameContentTarget_, camera.get());
    createAndSetRenderTarget(*editorCtx->gameContentTarget_, uiCtx->camera);

    cmds.emplace_resource<AssetPropertyWindowContext>();

    editorCtx->projectInfo =
        LoadProjectInfoFromFile(editorCtx->projectInfo.projectPath);

    auto editorConfig = editorCtx->projectInfo;
    editorConfig.windowData.size.Set(EditorWindowWidth, EditorWindowHeight);
    editorConfig.windowData.title =
        "NickelEngine Editor - " + editorCtx->projectInfo.windowData.title;
    InitProjectByConfig(editorConfig, window.get(), assetMgr.get());

    auto assetDir =
        GenAssetsDefaultStoreDir(editorCtx->projectInfo.projectPath);

    // init content browser info
    auto& contentBrowserInfo = cmds.emplace_resource<ContentBrowserInfo>();
    contentBrowserInfo.path = assetDir;
    contentBrowserInfo.rootPath = assetDir;
    if (!std::filesystem::exists(contentBrowserInfo.rootPath)) {
        if (!std::filesystem::create_directory(contentBrowserInfo.rootPath)) {
            LOGF(log_tag::Editor, "create resource dir ",
                 contentBrowserInfo.rootPath, " failed!");
        }
        // TODO: force quit editor
    }
    contentBrowserInfo.RescanDir();

    renderer->SetViewport({0, 0},
                          cgmath::Vec2(EditorWindowWidth, EditorWindowHeight));

    RegistComponentShowMethods();
    RegistSpawnMethods();
}

void EditorMenubar(EditorContext& ctx) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("save")) {
                SaveProjectByConfig(gWorld->res<EditorContext>()->projectInfo,
                                    gWorld->res<AssetManager>().get());
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("game content window", nullptr,
                            &ctx.openGameWindow);
            ImGui::MenuItem("inspector", nullptr, &ctx.openInspector);
            ImGui::MenuItem("entity list", nullptr, &ctx.openEntityList);
            ImGui::MenuItem("content browser", nullptr,
                            &ctx.openContentBrowser);
            ImGui::MenuItem("imgui demo window", nullptr, &ctx.openDemoWindow);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

struct DragDropInfo {
    gecs::entity dragEnt = gecs::null_entity;
    gecs::entity targetEnt = gecs::null_entity;
};

void showOneEntity(bool showNameBar, gecs::entity& selected, gecs::entity ent,
                   const Name& name, DragDropInfo& dragDropOutInfo) {
    if (showNameBar) {
        if (ImGui::Selectable(name.name.c_str(), selected == ent)) {
            selected = ent;
        }
    }

    // deal drag target
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("entity", &ent, sizeof(gecs::entity));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("entity")) {
            Assert(payload->DataSize == sizeof(gecs::entity),
                   "drag non-entity in entity list window");
            gecs::entity dragEnt = *(const gecs::entity*)payload->Data;
            dragDropOutInfo.targetEnt = ent;
            dragDropOutInfo.dragEnt = dragEnt;

            LOGW(log_tag::Editor, "targetEnt = ", ent, ", dragEnt = ", dragEnt);
        }
        ImGui::EndDragDropTarget();
    }
}

void showHierarchyEntities(gecs::entity entity, gecs::entity& selected,
                           const Name& name, const nickel::Child* children,
                           DragDropInfo& dragDropInfo) {
    if ((children && children->entities.empty()) || !children) {
        showOneEntity(true, entity, selected, name, dragDropInfo);
    } else {
        gecs::entity selectedEnt = gecs::null_entity;

        if (ImGui::TreeNodeEx((void*)name.name.c_str(),
                              ImGuiTreeNodeFlags_Selected |
                                  ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                  ImGuiTreeNodeFlags_SpanAvailWidth,
                              "%s", name.name.c_str())) {
            showOneEntity(false, entity, selected, name, dragDropInfo);
            for (auto ent : children->entities) {
                if (gWorld->cur_registry()->has<nickel::Name>(ent)) {
                    if (gWorld->cur_registry()->has<Child>(ent)) {
                        auto& child = gWorld->cur_registry()->get<Child>(ent);
                        auto& childName =
                            gWorld->cur_registry()->get<nickel::Name>(ent);
                        showHierarchyEntities(ent, selected, childName, &child,
                                              dragDropInfo);
                    } else {
                        auto& childName =
                            gWorld->cur_registry()->get<nickel::Name>(ent);
                        showHierarchyEntities(ent, selected, childName, nullptr,
                                              dragDropInfo);
                    }
                }
            }

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                selected = entity;
            }
            ImGui::TreePop();
        }
    }
}

void EditorEntityListWindow(bool& show, gecs::entity& selected,
                            gecs::registry reg, gecs::commands cmds) {
    if (!show) return;

    if (ImGui::Begin("Entity List", &show)) {
        if (ImGui::Button("add")) {
            auto ent = cmds.create();
            cmds.emplace<nickel::Name>(
                ent, nickel::Name{"entity-" +
                                  std::to_string(static_cast<uint32_t>(ent))});
        }
        if (reg.alive(selected)) {
            ImGui::SameLine();
            if (ImGui::Button("delete")) {
                cmds.destroy(selected);
            }
        }

        auto hierarchyEntities = gWorld->cur_registry()
                                     ->query<nickel::Child, nickel::Name,
                                             gecs::without<nickel::Parent>>();
        DragDropInfo dragDropInfo;
        for (auto&& [ent, child, name] : hierarchyEntities) {
            showHierarchyEntities(ent, selected, name, &child, dragDropInfo);
        }

        auto flatEntities =
            gWorld->cur_registry()->query<Name, gecs::without<Parent, Child>>();
        for (auto&& [ent, name] : flatEntities) {
            showOneEntity(true, selected, ent, name, dragDropInfo);
        }

        // drag and drop handle
        auto dragEnt = dragDropInfo.dragEnt;
        auto targetEnt = dragDropInfo.targetEnt;
        if (dragEnt != gecs::null_entity && targetEnt != gecs::null_entity) {
            nickel::Parent* parent = nullptr;
            if (reg.has<nickel::Parent>(dragEnt)) {
                parent = &reg.get_mut<nickel::Parent>(dragEnt);
                if (reg.alive(parent->entity)) {
                    auto& parentChildren =
                        reg.get_mut<nickel::Child>(parent->entity).entities;
                    parentChildren.erase(
                        std::remove(parentChildren.begin(),
                                    parentChildren.end(), dragEnt),
                        parentChildren.end());
                }
            } else {
                parent = &reg.commands().emplace<nickel::Parent>(dragEnt);
            }

            parent->entity = targetEnt;
            nickel::Child* children = nullptr;
            if (reg.has<nickel::Child>(targetEnt)) {
                children = &reg.get_mut<nickel::Child>(targetEnt);
            } else {
                children = &reg.commands().emplace<nickel::Child>(targetEnt);
            }
            children->entities.push_back(dragEnt);
        }
    }
    ImGui::End();
}

void EditorInspectorWindow(bool& show, gecs::entity entity, gecs::registry reg,
                           gecs::commands cmds) {
    if (!show) return;

    if (ImGui::Begin("Inspector", &show)) {
        auto& types = mirrow::drefl::all_typeinfo();

        if (!reg.alive(entity)) {
            ImGui::End();
            return;
        }

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
    }
    ImGui::End();
}

void DrawCoordLine(EditorContext& ctx, nickel::Renderer2D& renderer,
                   const Camera& camera, const Camera& uiCamera) {
    renderer.BeginRenderTexture(camera);
    cgmath::Rect rect{0, 0, ctx.projectInfo.windowData.size.w,
                      ctx.projectInfo.windowData.size.h};
    renderer.DrawRect(rect, {0, 0, 1, 1});
    // TODO: clip line start&end point to draw
    renderer.DrawLine({-10000, 0}, {10000, 0}, {1, 0, 0, 1});
    renderer.DrawLine({0, -10000}, {0, 10000}, {0, 1, 0, 1});

    renderer.EndRender();
}

void GameContentWindow(EditorContext& ctx, nickel::Renderer2D& renderer,
                       Camera& camera, Camera& uiCamera) {
    if (!ctx.openGameWindow) return;

    DrawCoordLine(ctx, renderer, camera, uiCamera);

    auto oldStyle = ImGui::GetStyle();
    auto newStyle = oldStyle;
    newStyle.WindowPadding.x = 0;
    newStyle.WindowPadding.y = 0;
    ImGui::GetStyle() = newStyle;

    if (ImGui::Begin("game", &ctx.openGameWindow)) {
        auto windowPos = ImGui::GetWindowPos();
        auto windowSize = ImGui::GetWindowSize();
        auto& gameWindowSize = ctx.projectInfo.windowData.size;
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)ctx.gameContentTexture_->Id(), windowPos,
            ImVec2{windowPos.x + windowSize.x, windowPos.y + windowSize.y},
            {0, windowSize.y / gameWindowSize.h},
            {windowSize.x / gameWindowSize.w, 0});

        auto& io = ImGui::GetIO();
        if (io.MouseWheel && ImGui::IsWindowHovered()) {
            auto scale = camera.Scale();
            constexpr float ScaleFactor = 0.02;
            constexpr float minScaleFactor = 0.0001;
            scale.x += ScaleFactor * io.MouseWheel;
            scale.y += ScaleFactor * io.MouseWheel;
            scale.x = scale.x < minScaleFactor ? minScaleFactor : scale.x;
            scale.y = scale.y < minScaleFactor ? minScaleFactor : scale.y;
            camera.SetScale(scale);

            scale = uiCamera.Scale();
            scale.x += ScaleFactor * io.MouseWheel;
            scale.y += ScaleFactor * io.MouseWheel;
            scale.x = scale.x < minScaleFactor ? minScaleFactor : scale.x;
            scale.y = scale.y < minScaleFactor ? minScaleFactor : scale.y;
            uiCamera.SetScale(scale);
        }
        if (io.MouseDelta.x != 0 && io.MouseDelta.y != 0 &&
            ImGui::IsWindowHovered() && io.MouseDown[ImGuiMouseButton_Left]) {
            auto offset = cgmath::Vec2{-io.MouseDelta.x, -io.MouseDelta.y};
            camera.As2D()->Move(offset);
            uiCamera.As2D()->Move(offset);
        }
    }
    ImGui::End();

    ImGui::GetStyle() = oldStyle;
}

void EditorImGuiUpdate(gecs::resource<gecs::mut<Renderer2D>> renderer,
                       gecs::resource<gecs::mut<EditorContext>> ctx,
                       gecs::resource<gecs::mut<ContentBrowserInfo>> cbInfo,
                       gecs::resource<gecs::mut<nickel::Camera>> camera,
                       gecs::resource<gecs::mut<nickel::ui::Context>> uiCtx,
                       gecs::registry reg, gecs::commands cmds) {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    EditorMenubar(ctx.get());

    GameContentWindow(ctx.get(), renderer.get(), camera.get(), uiCtx->camera);

    static gecs::entity selected = gecs::null_entity;
    EditorEntityListWindow(ctx->openEntityList, selected, reg, cmds);

    EditorInspectorWindow(ctx->openInspector, selected, reg, cmds);

    EditorContentBrowser(ctx->openContentBrowser);

    if (ctx->openDemoWindow) {
        ImGui::ShowDemoWindow(&ctx->openDemoWindow);
    }
}

void EditorExit() {
    gWorld->remove_res<ContentBrowserInfo>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo initInfo;
    initInfo.windowData.title = "Project Manager";
    initInfo.windowData.size.Set(ProjectMgrWindowWidth, ProjectMgrWindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.commands().emplace_resource<EditorContext>();

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
        .regist_exit_system_to_state<EditorExit>(EditorScene::Editor)
        .regist_update_system_to_state<plugin::ImGuiStart>(EditorScene::Editor)
        .regist_update_system_to_state<EditorImGuiUpdate>(EditorScene::Editor)
        .regist_update_system_to_state<plugin::ImGuiEnd>(EditorScene::Editor)
        // start with
        .start_with_state(EditorScene::ProjectManager);
}
