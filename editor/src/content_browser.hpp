#pragma once

#include "asset_property_window.hpp"
#include "file_dialog.hpp"
#include "image_view_canva.hpp"
#include "imgui_plugin.hpp"
#include "nickel.hpp"
#include "widget.hpp"


/**
 * @brief [Editor Event] triggered when asset release
 */
struct ReleaseAssetEvent {
    using HandleContainer =
        std::variant<nickel::TextureHandle, nickel::FontHandle,
                     nickel::AudioHandle>;

    explicit ReleaseAssetEvent(const HandleContainer& handle)
        : handle(handle) {}

    explicit ReleaseAssetEvent(const std::filesystem::path& path)
        : path(path) {}

    std::optional<HandleContainer> handle;
    std::optional<std::filesystem::path> path;
};

class ContentBrowserWindow : public Window {
public:
    ContentBrowserWindow();

    void Update() override;
    void RescanDir();

    void SetRootPath(const std::filesystem::path& root) { rootPath_ = root; }

    void SetCurPath(const std::filesystem::path& path) { path_ = path; }

    auto& CurPath() const { return path_; }

    auto& RootPath() const { return rootPath_; }

    nickel::Texture& FindTextureOrGen(const std::string& extension);

private:
    std::filesystem::path rootPath_;
    std::filesystem::path path_;
    ImageViewCanva imageViewer_;
    std::vector<std::filesystem::directory_entry> files_;
    const nickel::cgmath::Vec2 thumbnailSize_ = {32, 32};

    static constexpr float IconSize = 32;

    const std::filesystem::path iconConfigFilename_ =
        "./editor/resources/file_icon_map.toml";

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

    void selectAndLoadAsset();
    std::pair<const nickel::Texture&, bool> getIcon(
        const std::filesystem::directory_entry& entry,
        nickel::FileType filetype, nickel::AssetManager& assetMgr);

    void showOneIcon(nickel::AssetManager& assetMgr,
                     const std::filesystem::directory_entry& entry);

    void showIcons();

    void showAssetOperationPopupMenu(
        nickel::FileType filetype, bool hasImported,
        const std::filesystem::directory_entry& entry,
        nickel::AssetManager& assetMgr);

    void openPropWindowByFiletype(nickel::FileType);
};

void RegistEventHandler(gecs::event_dispatcher<ReleaseAssetEvent>);
