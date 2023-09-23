#pragma once

#include "nickel.hpp"
#include "pch.hpp"

namespace nickel {

struct ProjectInitInfo final {
    std::string projectPath;
    WindowBuilder::Data windowData = WindowBuilder::Data::Default();
};

void SaveProject(const std::string& rootPath, const TextureManager&,
                 const Window&);

/**
 * @brief save basic project information to file
 */
void SaveBasicProjectInfo(const std::string& rootPath, const ProjectInitInfo& initInfo);

/**
 * @brief save all assets information to file
 */
void SaveAssets(const std::string& rootPath, const TextureManager& textureMgr);

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
void InitProjectByConfig(const ProjectInitInfo&, Window& window, TextureManager& textureMgr);

/**
 * @brief init all inner ECS systems
 * @note you must call this before you do anything with engine(call in
 * `BootstrapSystem` in usual)
 */
void InitSystem(gecs::world& world, const ProjectInitInfo& info,
                gecs::commands cmds);
}  // namespace nickel