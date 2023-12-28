#pragma once

#include "asset_list_window.hpp"
#include "content_browser.hpp"
#include "core/gogl.hpp"
#include "entity_list_window.hpp"
#include "game_window.hpp"
#include "imgui_plugin.hpp"
#include "inspector.hpp"
#include "nickel.hpp"
#include "pch.hpp"
#include "tilesheet_editor.hpp"


struct EditorContext {
    // normal windows
    ContentBrowserWindow contentBrowserWindow;
    EntityListWindow entityListWindow;
    InspectorWindow inspectorWindow;
    GameWindow gameWindow;

    // popup windows
    TextureAssetListWindow textureAssetListWindow;
    FontAssetListWindow fontAssetListWindow;
    TilesheetAssetListWindow tilesheetAssetListWindow;
    TexturePropertyPopupWindow texturePropWindow;
    SoundPropertyPopupWindow soundPropWindow;
    TilesheetEditor tilesheetEditor;

    bool openGameWindow = true;
    bool openDemoWindow = false;

    // icon fonts
    // ImFont* imguiIconFont;

    // opened project information
    nickel::ProjectInitInfo projectInfo;

    void Update();

    std::filesystem::path GetRelativePath(
        const std::filesystem::path& path) const {
        return std::filesystem::relative(path, projectInfo.projectPath);
    }

    EditorContext();
    ~EditorContext();
};

inline void InitEditorContexta(gecs::commands cmds) {
    cmds.emplace_resource<EditorContext>();
}
