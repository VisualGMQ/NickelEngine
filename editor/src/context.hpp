#pragma once

#include "asset_list_window.hpp"
#include "content_browser.hpp"
#include "entity_list_window.hpp"
#include "game_window.hpp"
#include "imgui_plugin.hpp"
#include "inspector.hpp"
#include "nickel.hpp"
#include "tilesheet_editor.hpp"
#include "input_text_window.hpp"
#include "anim_editor.hpp"

struct EditorContext: public nickel::Singlton<EditorContext, true> {
private:
    std::filesystem::path editorPath_;

public:
    std::vector<Window*> globalWindows_;

    // normal windows
    ContentBrowserWindow contentBrowserWindow;
    EntityListWindow entityListWindow;
    InspectorWindow inspectorWindow;
    GameWindow gameWindow;
    AnimationEditor animEditor;

    // popup windows
    TextureAssetListWindow textureAssetListWindow;
    FontAssetListWindow fontAssetListWindow;
    TilesheetAssetListWindow tilesheetAssetListWindow;
    AnimationAssetListWindow animAssetListWindow;
    SoundAssetListWindow soundAssetListWindow;
    Material2DAssetListWindow mtl2dAssetListWindow;

    TilesheetEditor tilesheetEditor;
    InputTextWindow inputTextWindow;

    bool openDemoWindow = false;

    // icon fonts
    // ImFont* imguiIconFont;

    // opened project information
    nickel::ProjectInitInfo projectInfo;

    void Update();

    /**
     * @brief Get the relative path from project root path
     */
    std::filesystem::path GetRelativePath(
        const std::filesystem::path& path) const {
        return path.is_relative()
                   ? path
                   : std::filesystem::relative(path, projectInfo.projectPath);
    }

    auto& EditorPath() const { return editorPath_; }

    auto Convert2EditorRelatePath(const std::filesystem::path& p) const {
        return editorPath_ / p;
    }

    EditorContext();
    ~EditorContext();

    const nickel::TextCache& FindOrGenFontPrewview(nickel::FontHandle);

private:
    std::unordered_map<nickel::FontHandle, nickel::TextCache,
                       nickel::FontHandle::Hash, nickel::FontHandle::Eq>
        fontPreviewTextures_;
    std::unordered_map<nickel::SoundHandle, nickel::SoundPlayer,
                       nickel::SoundHandle::Hash, nickel::SoundHandle::Eq>
        soundPlayers_;

    void updateMenubar();
};

inline void InitEditorContext(gecs::commands cmds) {
    EditorContext::Init();
}
