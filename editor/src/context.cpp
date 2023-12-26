#include "context.hpp"
#include "config.hpp"
#include "core/gogl.hpp"

EditorContext::EditorContext()
    : textureAssetListWindow("textures"),
      fontAssetListWindow("fonts"),
      texturePropWindow("texture property"),
      soundPropWindow("sound property") {
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
    texturePropWindow.Update();
    soundPropWindow.Update();
}

EditorContext::~EditorContext() {
}