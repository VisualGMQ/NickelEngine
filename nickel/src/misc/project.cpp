#include "misc/project.hpp"
#include "common/log_tag.hpp"
#include "graphics/gltf.hpp"
#include "graphics/system.hpp"
#include "mirrow/drefl/make_any.hpp"
#include "misc/serd.hpp"
#include "nickel.hpp"
#include "audio/audio.hpp"
#include "refl/drefl.hpp"
#include "system/graphics.hpp"
#include "system/physics.hpp"
#include "system/video.hpp"
#include "ui/context.hpp"
#include "common/asset_manager.hpp"

namespace nickel {

void SaveAssets(const std::filesystem::path& rootPath) {
    // TODO: change root path
    AssetManager::Instance().SaveAllAssets();
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

ProjectInitInfo CreateNewProject(const std::filesystem::path& dir) {
    std::filesystem::create_directories(GenAssetsDefaultStoreDir(dir));
    ProjectInitInfo initInfo;
    initInfo.projectPath = dir;
    initInfo.windowData.title = "new project";
    initInfo.windowData.size.Set(720, 680);
    SaveProjectByConfig(initInfo);

    LOGI(log_tag::Nickel, "Create new project to ", dir);

    return initInfo;
}

void LoadAssetsWithPath(const std::filesystem::path& configDir) {
    // TODO: not finish
}

bool LoadScene(gecs::registry reg, const std::filesystem::path& filename) {
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

    toml::table windowTbl;

    mirrow::serd::drefl::serialize(
        windowTbl, mirrow::drefl::any_make_constref(initInfo.windowData),
        "window");
    // TODO: serialize camera information

    tbl.emplace("scene", initInfo.startupScene.string());

    std::ofstream file(GenProjectConfigFilePath(initInfo.projectPath));
    file << toml::toml_formatter{tbl} << std::endl;
}

void SaveProjectByConfig(const ProjectInitInfo& info) {
    SaveBasicProjectConfig(info);
    SaveAssets(info.projectPath);
    SaveRegistry(true, info.projectPath, "MainScene.scene",
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

    if (auto node = tbl["scene"]; node.is_string()) {
        initInfo.startupScene = node.as_string()->get();
    }

    return initInfo;
}

void LoadProject(const std::string& rootPath, Window& window) {
    ProjectInitInfo initInfo = LoadProjectInfoFromFile(rootPath);

    InitProjectByConfig(initInfo, window);
}

void InitProjectByConfig(const ProjectInitInfo& initInfo, Window& window) {
    window.Resize(initInfo.windowData.size.w, initInfo.windowData.size.h);
    window.SetTitle(initInfo.windowData.title);
    LoadAssetsWithPath(initInfo.projectPath);
    LoadScene(*ECS::Instance().World().cur_registry(),
              initInfo.projectPath / initInfo.startupScene);
}

void doChangeScene(const ChangeSceneEvent& event) {
    auto& world = ECS::Instance().World();
    if (std::filesystem::exists(event.newScene)) {
        world.cur_registry()->destroy_all_entities();
        LoadScene(*world.cur_registry(), event.newScene);
    } else {
        LOGE(log_tag::Nickel, "scene file ", event.newScene, " not exists");
    }
}

void ChangeScene(const std::filesystem::path& path) {
    ECS::Instance()
        .World()
        .cur_registry()
        ->event_dispatcher<ChangeSceneEvent>()
        .enqueue(ChangeSceneEvent{path});
}

void InitSystem(gecs::world& world, const ProjectInitInfo& info,
                gecs::commands cmds) {
    RegistReflectInfos();
    RegistSerializeMethods();
    RegistComponents();

    AssetManager::Init();

    auto renderAPI = rhi::GetSupportRenderAPI(rhi::APIPreference::Vulkan);
    Window& window =
        cmds.emplace_resource<Window>(WindowBuilder{info.windowData}.Build(
            renderAPI == rhi::APIPreference::Vulkan));
    window.SetResizable(true);

    RenderContext::Init(window, rhi::Adapter::Option{renderAPI});
    ui::UIContext::Init(window.Size());
    DataPool::Init();

    cmds.emplace_resource<Time>();

    auto windowSize = window.Size();

    cmds.emplace_resource<Camera>(
        Camera::CreateOrthoByWindowRegion(window.Size()));

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
    FontSystemShutdown();
    world.remove_res<RenderContext>();
    DataPool::Delete();
    ui::UIContext::Delete();
    RenderContext::Delete();
    AssetManager::Delete();
}

void RegistEngineSystem(typename gecs::world::registry_type& reg) {
    reg
        // startup systems
        .regist_startup_system<VideoSystemInit>()
        .regist_startup_system<RenderSystemInit>()
        .regist_startup_system<FontSystemInit>()
        .regist_startup_system<EventPollerInit>()
        .regist_startup_system<InputSystemInit>()
        .regist_startup_system<ui::InitSystem>()
        .regist_startup_system<InitAudioSystem>()
        // shutdown systems
        .regist_shutdown_system<ui::ShutdownSystem>()
        // .regist_shutdown_system<ScriptShutdownSystem>()
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
        .regist_update_system<ui::UpdateGlobalPosition>()
        .regist_update_system<ui::HandleEventSystem>()
        // render relate
        .regist_update_system<BeginFrame>()
        .regist_update_system<BeginRender>()
        .regist_update_system<RenderGLTFModel>()
        .regist_update_system<RenderSprite2D>()
        .regist_update_system<ui::RenderUI>()
        .regist_update_system<EndRender>()
        .regist_update_system<EndFrame>()
        // time update
        .regist_update_system<Time::Update>();
        // regist_update_system<ScriptUpdateSystem>();

    reg.event_dispatcher<ChangeSceneEvent>().sink().add<doChangeScene>();
}

}  // namespace nickel
