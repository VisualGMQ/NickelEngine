#pragma once

#include "context.hpp"
#include "file_dialog.hpp"
#include "imgui_plugin.hpp"
#include "misc/filetype.hpp"
#include "nickel.hpp"

struct ContentBrowserInfo {
    std::filesystem::path rootPath;
    std::filesystem::path path;
    nickel::cgmath::Vec2 thumbnailSize = {32, 32};
    const std::string texturePropertyPopupWindowTitle = "texture property";

    void RescanDir();

    auto& Files() const { return files_; }

    nickel::Texture& FindTextureOrGen(const std::string& extension);

    const nickel::Texture& GetDirIcon() const {
        return textureMgr_.Get(dirIconHandle_);
    }

    ContentBrowserInfo();

private:
    const nickel::cgmath::Vec2 iconSize_ = {200, 200};
    const std::filesystem::path iconConfigFilename_ = "./editor/resources/file_icon_map.toml";

    std::vector<std::filesystem::directory_entry> files_;

    std::unordered_map<std::string, nickel::TextureHandle> extensionHandleMap_;
    std::unordered_map<std::string, std::filesystem::path> extensionIconMap_;
    nickel::TextureManager textureMgr_;
    nickel::TextureHandle dirIconHandle_;
    nickel::TextureHandle unknownFileIconHandle_;

    void initExtensionIconMap();
    void registFileIcon(const std::string& extension,
                        const std::filesystem::path& svgPath) {
        extensionIconMap_.insert_or_assign(extension, svgPath);
    }
};

void EditorContentBrowser(bool& show);
void SelectAndLoadAsset(gecs::registry);
