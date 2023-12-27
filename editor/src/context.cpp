#include "context.hpp"
#include "config.hpp"
#include "core/gogl.hpp"

EditorContext::EditorContext()
    : textureAssetListWindow("textures"),
      fontAssetListWindow("fonts"),
      tilesheetAssetListWindow("tilesheets"),
      texturePropWindow("texture property"),
      soundPropWindow("sound property"),
      tilesheetEditor("tilesheet editor") {
    contentBrowserWindow.SetTitle("content browser");
    entityListWindow.SetTitle("entity list");
    inspectorWindow.SetTitle("inspector");
}

void EditorContext::Update() {
    contentBrowserWindow.Update();
    entityListWindow.Update();
    inspectorWindow.Update();
    gameWindow.Update();
    textureAssetListWindow.Update();
    fontAssetListWindow.Update();
    tilesheetAssetListWindow.Update();
    texturePropWindow.Update();
    soundPropWindow.Update();
    tilesheetEditor.Update();
}

EditorContext::~EditorContext() {
}