#include "misc/project.hpp"
#include "refl/drefl.hpp"
#include "refl/window.hpp"
#include "renderer/texture.hpp"

namespace nickel {

void SaveAssets(const std::string& rootPath, const TextureManager& textureMgr) {
    toml::table tbl;
    auto textureMgrTbl = textureMgr.Save2Toml();
    tbl.emplace("textures", textureMgrTbl);

    std::ofstream file(rootPath + "/assets.toml");
    file << toml::toml_formatter{tbl} << std::flush;
}

void SaveBasicProjectInfo(const std::string& rootPath, const ProjectInitInfo& initInfo) {
    toml::table tbl;

    tbl.emplace("window", mirrow::serd::srefl::serialize(initInfo.windowData));

    // TODO: serialize camera information

    std::ofstream file(rootPath + "/project.toml");
    file << toml::toml_formatter{tbl} << std::endl;
}

void SaveProject(const std::string& rootPath, const TextureManager& textureMgr,
                 const Window& window) {
    ProjectInitInfo initInfo;
    initInfo.projectPath = rootPath;
    initInfo.windowData.title = window.Title();
    initInfo.windowData.size = window.Size();
    SaveBasicProjectInfo(rootPath, initInfo);
    SaveAssets(rootPath, textureMgr);
}

void SaveProjectConfig(const std::string& path, const ProjectInitInfo& info) {

}

void LoadAssets(const std::string& rootPath,
                        TextureManager& textureMgr) {
    auto result = toml::parse_file(rootPath + "/assets.toml");
    if (!result) {
        LOGE(log_tag::Res, "load saved textures failed: ", result.error());
        return;
    }

    auto& tbl = result.table();
    if (auto textureMgrTbl = tbl["textures"]; textureMgrTbl.is_table()) {
        textureMgr.LoadFromToml(*textureMgrTbl.as_table());
    }
}

ProjectInitInfo LoadBasicProjectConfig(const std::string& rootPath) {
    ProjectInitInfo initInfo;

    initInfo.projectPath = rootPath;
    std::filesystem::path path(rootPath + "/project.toml");
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
        mirrow::serd::srefl::deserialize(*node.as_table(), initInfo.windowData);
    }

    return initInfo;
}

void LoadProject(const std::string& rootPath, Window& window,
                 TextureManager& textureMgr) {
    ProjectInitInfo initInfo = LoadBasicProjectConfig(rootPath);

    InitProjectByConfig(initInfo, window, textureMgr);
}

void InitProjectByConfig(const ProjectInitInfo& initInfo, Window& window, TextureManager& textureMgr) {
    window.Resize(initInfo.windowData.size.w, initInfo.windowData.size.h);
    window.SetTitle(initInfo.windowData.title);
    LoadAssets(initInfo.projectPath, textureMgr);
}

void InitSystem(gecs::world& world, const ProjectInitInfo& info,
                gecs::commands cmds) {
    InitDynamicReflect();

    Window* window =
        &cmds.emplace_resource<Window>(WindowBuilder{info.windowData}.Build());
    cmds.emplace_resource<Camera>(Camera2D::Default(*window));

    cmds.emplace_resource<EventPoller>(EventPoller{});
    EventPoller::AssociatePollerAndECS(*world.cur_registry());
    EventPoller::ConnectPoller2Events(*window);

    cmds.emplace_resource<Time>();
    cmds.emplace_resource<TextureManager>();
    cmds.emplace_resource<TimerManager>();
    cmds.emplace_resource<AnimationManager>();

    auto windowSize = window->Size();

    auto& renderer2d = cmds.emplace_resource<Renderer2D>();
    renderer2d.SetViewport(cgmath::Vec2{0, 0}, windowSize);

    // init animation serialize method
    AnimTrackSerialMethods::Instance()
        .RegistMethod<cgmath::Vec2>()
        .RegistMethod<cgmath::Vec3>()
        .RegistMethod<float>()
        .RegistMethod<double>()
        .RegistMethod<int>()
        .RegistMethod<long>();
}

}  // namespace nickel
