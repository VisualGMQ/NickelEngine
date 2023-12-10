#pragma once

#include "context.hpp"
#include "imgui_plugin.hpp"
#include "nickel.hpp"
#include "file_dialog.hpp"
#include "filetype.hpp"

struct ContentBrowserInfo {
    std::filesystem::path rootPath;
    std::filesystem::path path;
    nickel::cgmath::Vec2 thumbnailSize = {32, 32};

    void RescanDir();

    auto& Files() const { return files_; }

    nickel::Texture& FindTextureOrGen(const std::string& extension);
    const nickel::Texture& GetDirIcon() const { return textureMgr_.Get(dirIconHandle_); }

    void RegistFileIcon(const std::string& extension,
                        const std::filesystem::path& svgPath) {
        extensionIconMap_.insert_or_assign(extension, svgPath);
    }

    ContentBrowserInfo();

private:
    const nickel::cgmath::Vec2 iconSize_ = {200, 200};

    std::vector<std::filesystem::directory_entry> files_;

    std::unordered_map<std::string, nickel::TextureHandle> extensionHandleMap_;
    std::unordered_map<std::string, std::filesystem::path> extensionIconMap_;
    nickel::TextureManager textureMgr_;
    nickel::TextureHandle dirIconHandle_;
};

void EditorContentBrowser(bool& show);
