#pragma once

#include "core/gogl.hpp"
#include "pch.hpp"
#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "content_browser.hpp"
#include "entity_list_window.hpp"
#include "inspector.hpp"
#include "asset_list_window.hpp"
#include "game_window.hpp"
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

    EditorContext();
    ~EditorContext();

};

inline void InitEditorContexta(gecs::commands cmds) {
    cmds.emplace_resource<EditorContext>();
}
