#pragma once

#include "anim/anim.hpp"
#include "misc/asset_manager.hpp"
#include "misc/filetype.hpp"
#include "misc/timer.hpp"
#include "pch.hpp"
#include "renderer/camera.hpp"
#include "renderer/font.hpp"
#include "renderer/renderer2d.hpp"
#include "renderer/texture.hpp"
#include "window/event.hpp"
#include "window/window.hpp"


namespace nickel {

constexpr std::string_view AssetFilename = "asset.toml";

struct ProjectInitInfo final {
    std::filesystem::path projectPath;
    WindowBuilder::Data windowData = WindowBuilder::Data::Default();
};

void SaveProject(const std::filesystem::path& rootPath, const AssetManager&,
                 const Window&);

/**
 * @brief save basic project information to file
 */
void SaveBasicProjectConfig(const std::filesystem::path& rootPath,
                          const ProjectInitInfo& initInfo);

/**
 * @brief save all assets information to file
 */
void SaveAssets(const std::filesystem::path& rootPath,
                const AssetManager&);

/**
 * @brief load all assets from file
 */
void LoadAssets(const std::filesystem::path& rootPath, AssetManager&);

/**
 * @brief load basic project config
 *
 * @param rootPath project root directory path
 * @return ProjectInitInfo project config
 */
ProjectInitInfo LoadProjectInfoFromFile(const std::filesystem::path& rootPath);

/**
 * @brief load project config from file and init project
 *
 * @param rootPath project root directory path
 */
void LoadProject(const std::string& rootPath, Window& window, AssetManager&);

/**
 * @brief init project from ProjectInitInfo
 */
void InitProjectByConfig(const ProjectInitInfo&, Window& window,
                         AssetManager&);

/**
 * @brief init all inner ECS systems
 * @note you must call this before you do anything with engine(call in
 * `BootstrapSystem` usually)
 */
void InitSystem(gecs::world& world, const ProjectInitInfo& info,
                gecs::commands cmds);

constexpr std::string_view ResDir = "resources";

/**
 * @brief get resource dir path from project root path
 *
 * @param root project root path
 * @return std::filesystem::path
 */
inline std::filesystem::path GenResourcePath(
    const std::filesystem::path& root) {
    return root / std::filesystem::path{ResDir};
}

constexpr std::string_view ProjectConfigFile = "project.toml";

inline std::filesystem::path GenProjectConfigFilePath(
    const std::filesystem::path& root) {
    return root / std::filesystem::path{ProjectConfigFile};
}

}  // namespace nickel
