#pragma once

#include "anim/anim.hpp"
#include "misc/timer.hpp"
#include "pch.hpp"
#include "renderer/camera.hpp"
#include "renderer/renderer2d.hpp"
#include "renderer/texture.hpp"
#include "window/event.hpp"
#include "window/window.hpp"

namespace nickel {

struct ProjectInitInfo final {
    std::string projectPath;
    WindowBuilder::Data windowData = WindowBuilder::Data::Default();
};

void SaveProject(const std::filesystem::path& rootPath, const TextureManager&,
                 const Window&);

/**
 * @brief save basic project information to file
 */
void SaveBasicProjectInfo(const std::filesystem::path& rootPath,
                          const ProjectInitInfo& initInfo);

/**
 * @brief save all assets information to file
 */
void SaveAssets(const std::filesystem::path& rootPath, const TextureManager& textureMgr);

/**
 * @brief load basic project config
 *
 * @param rootPath project root directory path
 * @return ProjectInitInfo project config
 */
ProjectInitInfo LoadBasicProjectConfig(const std::string& rootPath);

/**
 * @brief load all assets from file
 */
void LoadAssets(const std::string& rootPath, TextureManager& textureMgr);

/**
 * @brief load project config from file and init project
 *
 * @param rootPath project root directory path
 */
void LoadProject(const std::string& rootPath, Window& window, TextureManager&);

/**
 * @brief init project from ProjectInitInfo
 */
void InitProjectByConfig(const ProjectInitInfo&, Window& window,
                         TextureManager& textureMgr);

/**
 * @brief init all inner ECS systems
 * @note you must call this before you do anything with engine(call in
 * `BootstrapSystem` in usual)
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

}  // namespace nickel