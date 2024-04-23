#include "misc/project.hpp"
#include "common/log_tag.hpp"
#include "graphics/gltf.hpp"
#include "graphics/system.hpp"
#include "mirrow/drefl/make_any.hpp"
#include "nickel.hpp"
#include "refl/drefl.hpp"
#include "system/graphics.hpp"
#include "system/physics.hpp"
#include "system/video.hpp"


namespace nickel {

void SaveAssets(const std::filesystem::path& rootPath,
                const AssetManager& assetMgr) {
    assetMgr.Save2TomlFile(rootPath, rootPath / AssetFilename);
    assetMgr.SaveAssets2File();
}

toml::table SaveRegistryToToml(std::string_view name, gecs::registry reg) {
    auto& entities = reg.entities();

    toml::table root;
    toml::array arr;

    root.emplace("name", name);

    for (int i = 0; i < entities.size(); i++) {
        auto ent = static_cast<gecs::entity>(entities.packed()[i]);
        if (reg.has<nickel::Parent>(ent)) {
            continue;
        }
        auto entityNode = SaveAsPrefab(static_cast<gecs::entity>(ent), reg);
        if (!entityNode.empty()) {
            arr.push_back(entityNode);
        }
    }

    root.emplace("entities", arr);

    return root;
}

void SaveRegistry(bool isMainScene, const std::filesystem::path& rootPath,
                  const std::string_view sceneName, gecs::registry reg) {
    std::ofstream file(
        rootPath /
        std::filesystem::path{sceneName}.replace_extension(SceneFileExtension));
    auto tbl = SaveRegistryToToml(sceneName, reg);
    file << tbl;
}

ProjectInitInfo CreateNewProject(const std::filesystem::path& dir,
                                 AssetManager& assetMgr) {
    std::filesystem::create_directories(GenAssetsDefaultStoreDir(dir));
    ProjectInitInfo initInfo;
    initInfo.projectPath = dir;
    initInfo.windowData.title = "new project";
    initInfo.windowData.size.Set(720, 680);
    SaveProjectByConfig(initInfo, assetMgr);

    LOGI(log_tag::Nickel, "Create new project to ", dir);

    return initInfo;
}

void LoadAssetsWithPath(AssetManager& assetMgr,
                        const std::filesystem::path& configDir) {
    auto path = GenAssetsConfigFilePath(configDir);
    auto result = toml::parse_file(path.string());
    if (!result) {
        LOGE(log_tag::Asset, "load saved textures from ", path,
             " failed: ", result.error());
        return;
    }

    assetMgr.LoadFromToml(result.table());
}

bool LoadRegistryEntities(gecs::registry reg,
                          const std::filesystem::path& filename) {
    auto result = toml::parse_file(filename.string());
    if (!result) {
        LOGW(nickel::log_tag::Editor, "load scene from ", filename, " failed");
        return false;
    }

    auto& tbl = result.table();
    auto node = tbl.get("name");
    if (!node || !node->is_string()) {
        LOGW(nickel::log_tag::Editor, "can't load anonymous scene");
        return false;
    }

    const std::string& name = node->as_string()->get();

    // InitSystem to reg

    node = tbl.get("entities");
    if (!node || !node->is_array()) {
        LOGW(nickel::log_tag::Editor,
             "`entities` don't exists or not array of tables");
        return false;
    }

    auto& entityList = *node->as_array();
    for (auto& entityTbl : entityList) {
        CreateFromPrefab(*entityTbl.as_array(), reg);
    }

    return true;
}

void SaveBasicProjectConfig(const ProjectInitInfo& initInfo) {
    toml::table tbl;

    mirrow::serd::drefl::serialize(
        tbl, mirrow::drefl::any_make_constref(initInfo.windowData), "window");
    // TODO: serialize camera information

    std::ofstream file(GenProjectConfigFilePath(initInfo.projectPath));
    file << toml::toml_formatter{tbl} << std::endl;
}

void SaveProjectByConfig(const ProjectInitInfo& info,
                         const AssetManager& assetMgr) {
    SaveBasicProjectConfig(info);
    SaveAssets(info.projectPath, assetMgr);
    SaveRegistry(true, info.projectPath,
                 ECS::Instance().World().cur_registry_name(),
                 *ECS::Instance().World().cur_registry());
}

ProjectInitInfo LoadProjectInfoFromFile(const std::filesystem::path& rootPath) {
    ProjectInitInfo initInfo;

    initInfo.projectPath = rootPath;
    std::filesystem::path path(GenProjectConfigFilePath(rootPath));
    if (!std::filesystem::exists(path)) {
        LOGF(log_tag::Nickel, "project config file ", path.string(),
             " not exists");
        return initInfo;
    }

    auto result = toml::parse_file(path.string());
    if (result.failed()) {
        LOGF(log_tag::Nickel, "load project failed: ", result.error());
        return initInfo;
    }

    auto& tbl = result.table();
    if (auto node = tbl["window"]; node.is_table()) {
        auto ref = mirrow::drefl::any_make_ref(initInfo.windowData);
        mirrow::serd::drefl::deserialize(ref, *node.as_table());
    }

    return initInfo;
}

void LoadProject(const std::string& rootPath, Window& window,
                 AssetManager& assetMgr) {
    ProjectInitInfo initInfo = LoadProjectInfoFromFile(rootPath);

    InitProjectByConfig(initInfo, window, assetMgr);
}

void InitProjectByConfig(const ProjectInitInfo& initInfo, Window& window,
                         AssetManager& assetMgr) {
    window.Resize(initInfo.windowData.size.w, initInfo.windowData.size.h);
    window.SetTitle(initInfo.windowData.title);
    LoadAssetsWithPath(assetMgr, initInfo.projectPath);
    LoadRegistryEntities(*ECS::Instance().World().cur_registry(),
                         initInfo.projectPath / "MainReg.scene");
}

void ErrorCallback(int error, const char* description) {
    LOGE(log_tag::SDL2, description);
}

void InitSystem(gecs::world& world, const ProjectInitInfo& info,
                gecs::commands cmds) {
    RegistReflectInfos();

    auto renderAPI = rhi::GetSupportRenderAPI(rhi::APIPreference::Vulkan);
    Window& window = cmds.emplace_resource<Window>(
        WindowBuilder{info.windowData}.Build(renderAPI ==
                                             rhi::APIPreference::Vulkan));
    window.SetResizable(true);

    auto& adapter = cmds.emplace_resource<rhi::Adapter>(
        window.Raw(), rhi::Adapter::Option{renderAPI});
    auto& device = cmds.emplace_resource<rhi::Device>(adapter.RequestDevice());

    cmds.emplace_resource<Time>();
    auto& textureMgr = cmds.emplace_resource<TextureManager>();
    auto& mtl2dMgr = cmds.emplace_resource<Material2DManager>();
    auto& fontMgr = cmds.emplace_resource<FontManager>();
    auto& timerMgr = cmds.emplace_resource<TimerManager>();
    auto& tilesheetMgr = cmds.emplace_resource<TilesheetManager>();
    auto& animMgr = cmds.emplace_resource<AnimationManager>();
    auto& audioMgr = cmds.emplace_resource<AudioManager>();
    auto& gltfMgr = cmds.emplace_resource<GLTFManager>();
    cmds.emplace_resource<AssetManager>(textureMgr, mtl2dMgr, fontMgr, timerMgr,
                                        tilesheetMgr, animMgr, audioMgr, gltfMgr);
    cmds.emplace_resource<RenderContext>(renderAPI, device, window.Size());

    auto windowSize = window.Size();

    cmds.emplace_resource<Camera>(
        Camera::CreateOrthoByWindowRegion(window.Size()));
    // renderer2d.SetViewport(cgmath::Vec2{0, 0}, windowSize);
    // world.cur_registry()
    //     ->event_dispatcher<WindowResizeEvent>()
    //     .sink()
    //     .add<suitCanvas2Window>();

    // init animation serialize method
    AnimTrackLoadMethods::Instance()
        .RegistMethod<cgmath::Vec2>()
        .RegistMethod<cgmath::Vec3>()
        .RegistMethod<cgmath::Vec4>()
        .RegistMethod<float>()
        .RegistMethod<double>()
        .RegistMethod<int>()
        .RegistMethod<long>();
}

void EngineShutdown() {
    PROFILE_BEGIN();

    auto& world = ECS::Instance().World();

    world.destroy_all_entities();
    world.remove_res<TilesheetManager>();
    world.remove_res<TextureManager>();
    world.remove_res<Material2DManager>();
    world.remove_res<FontManager>();
    world.remove_res<TimerManager>();
    world.remove_res<AnimationManager>();
    world.remove_res<AudioManager>();
    world.remove_res<GLTFManager>();
    FontSystemShutdown();
    world.remove_res<RenderContext>();
    world.res_mut<rhi::Device>()->Destroy();
    world.res_mut<rhi::Adapter>()->Destroy();
}

void RegistEngineSystem(typename gecs::world::registry_type& reg) {
    reg
        // startup systems
        .regist_startup_system<VideoSystemInit>()
        .regist_startup_system<RenderSystemInit>()
        .regist_startup_system<FontSystemInit>()
        .regist_startup_system<EventPollerInit>()
        .regist_startup_system<InputSystemInit>()
        // .regist_startup_system<ui::InitSystem>()
        .regist_startup_system<InitAudioSystem>()
        // shutdown systems
        .regist_shutdown_system<EngineShutdown>()
        // update systems
        .regist_update_system<VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<Mouse::Update>()
        .regist_update_system<Keyboard::Update>()
        .regist_update_system<HandleInputEvents>()
        .regist_update_system<UpdateGlobalTransform>()
        .regist_update_system<UpdateGLTFModelTransform>()
        .regist_update_system<UpdateCamera2GPU>()
        // .regist_update_system<ui::UpdateGlobalPosition>()
        // .regist_update_system<ui::HandleEventSystem>()
        // start render pipeline
        .regist_update_system<BeginFrame>()
        .regist_update_system<BeginRender>()
        .regist_update_system<RenderGLTFModel>()
        .regist_update_system<RenderSprite2D>()
        .regist_update_system<EndRender>()
        .regist_update_system<EndFrame>()
        // 2D UI render
        // .regist_update_system<ui::RenderUI>()
        // time update
        .regist_update_system<Time::Update>();
}

}  // namespace nickel
