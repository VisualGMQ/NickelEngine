#include "misc/project.hpp"
#include "core/log_tag.hpp"
#include "mirrow/drefl/make_any.hpp"
#include "misc/asset_manager.hpp"
#include "refl/drefl.hpp"
#include "renderer/context.hpp"
#include "renderer/font.hpp"
#include "renderer/texture.hpp"
#include "ui/context.hpp"

namespace nickel {

void SaveAssets(const std::filesystem::path& rootPath,
                const AssetManager& assetMgr) {
    assetMgr.Save2TomlFile(rootPath, rootPath / AssetFilename);
}

ProjectInitInfo CreateNewProject(const std::filesystem::path& dir, AssetManager& assetMgr) {
    std::filesystem::create_directories(GenAssetsDefaultStoreDir(dir));
    ProjectInitInfo initInfo;
    initInfo.projectPath = dir;
    initInfo.windowData.title = "new project";
    initInfo.windowData.size.Set(720, 680);
    assetMgr.SetRootPath(GenAssetsDefaultStoreDir(dir));
    SaveProjectByConfig(initInfo, assetMgr);

    LOGI(log_tag::Nickel, "Create new project to ", dir);

    return initInfo;
}

void LoadAssetsWithPath(AssetManager& assetMgr, const std::filesystem::path& configDir) {
    auto path = GenAssetsConfigFilePath(configDir);
    auto result = toml::parse_file(path.string());
    if (!result) {
        LOGE(log_tag::Asset, "load saved textures from ", path,
             " failed: ", result.error());
        return;
    }

    assetMgr.LoadFromTomlWithPath(result.table(), configDir);
}

void SaveBasicProjectConfig(const ProjectInitInfo& initInfo) {
    toml::table tbl;

    mirrow::serd::drefl::serialize(
        tbl, mirrow::drefl::any_make_constref(initInfo.windowData), "window");
    // TODO: serialize camera information

    std::ofstream file(GenProjectConfigFilePath(initInfo.projectPath));
    file << toml::toml_formatter{tbl} << std::endl;
}

void SaveProjectByConfig(const ProjectInitInfo& info, const AssetManager& assetMgr) {
    SaveBasicProjectConfig(info);
    SaveAssets(info.projectPath, assetMgr);
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
}

void ErrorCallback(int error, const char* description) {
    LOGE(log_tag::SDL2, description);
}

void suitCanvas2Window(const WindowResizeEvent& event,
                       gecs::resource<gecs::mut<Camera>> camera,
                       gecs::resource<gecs::mut<ui::Context>> uiCtx,
                       gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    if (camera->GetType() == Camera::Type::Dimension2) {
        auto camera2d = camera->As2D();
        camera2d->SetProject(0.0, event.size.w, 0.0, event.size.h, 1000.0,
                             -1000.0);
        renderer2d->SetViewport(cgmath::Vec2{0, 0},
                                cgmath::Vec2(event.size.w, event.size.h));
    } else {
        // TODO: change Camera3D project
    }

    uiCtx->camera.As2D()->SetProject(0.0, event.size.w, 0.0, event.size.h,
                                     1000.0, -1000.0);
}

void InitSystem(gecs::world& world, const ProjectInitInfo& info,
                gecs::commands cmds) {
    InitDynamicReflect();

    Window* window =
        &cmds.emplace_resource<Window>(WindowBuilder{info.windowData}.Build());

    cmds.emplace_resource<Time>();
    cmds.emplace_resource<AssetManager>();
    cmds.emplace_resource<FontManager>();
    cmds.emplace_resource<AnimationManager>();
    cmds.emplace_resource<TimerManager>();
    cmds.emplace_resource<RenderContext>();

    auto windowSize = window->Size();

    auto& renderer2d = cmds.emplace_resource<Renderer2D>();
    cmds.emplace_resource<Camera>(Camera2D::Default(*window));
    renderer2d.SetViewport(cgmath::Vec2{0, 0}, windowSize);
    world.cur_registry()
        ->event_dispatcher<WindowResizeEvent>()
        .sink()
        .add<suitCanvas2Window>();

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

}  // namespace nickel
