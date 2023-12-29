#include "context.hpp"
#include "config.hpp"
#include "core/gogl.hpp"

EditorContext::EditorContext()
    : textureAssetListWindow("textures"),
      fontAssetListWindow("fonts"),
      tilesheetAssetListWindow("tilesheets"),
      texturePropWindow("texture property"),
      soundPropWindow("sound property"),
      fontPropWindow("font property"),
      tilesheetEditor("tilesheet editor"),
      contentBrowserWindow(this),
      editorPath_{std::filesystem::current_path()} {
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
    fontPropWindow.Update();
    tilesheetAssetListWindow.Update();
    texturePropWindow.Update();
    soundPropWindow.Update();
    tilesheetEditor.Update();
}

EditorContext::~EditorContext() {}

const nickel::TextCache& EditorContext::FindOrGenFontPrewview(
    nickel::FontHandle handle) {
    if (auto it = fontPreviewTextures_.find(handle);
        it != fontPreviewTextures_.end()) {
        return it->second;
    }

    auto assetMgr = gWorld->res<nickel::AssetManager>();
    if (!assetMgr->Has(handle)) {
        return nickel::TextCache::Null;
    }

    auto& font = assetMgr->Get(handle);
    nickel::TextCache cache;
    constexpr std::string_view text = "the brown fox jumps over the lazy dog";
    constexpr int ptSize = 40;
    for (auto c : text) {
        cache.Push(nickel::Character(font.GetGlyph(c, ptSize)));
    }
    return fontPreviewTextures_.emplace(handle, std::move(cache)).first->second;
}