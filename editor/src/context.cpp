#include "context.hpp"

EditorContext::EditorContext()
    : textureAssetListWindow("textures"),
      fontAssetListWindow("fonts"),
      tilesheetAssetListWindow("tilesheets"),
      soundAssetListWindow("sounds"),
      animAssetListWindow("animations"),
      mtl2dAssetListWindow("2D materials"),
      tilesheetEditor("tilesheet editor"),
      inputTextWindow("input text"),
      contentBrowserWindow(this),
      scriptAssetListWindow("scripts"),
      editorPath_{std::filesystem::current_path()} {
    contentBrowserWindow.SetTitle("content browser");
    entityListWindow.SetTitle("entity list");
    inspectorWindow.SetTitle("inspector");
    gameWindow.SetTitle("game");
    initGameWindowTexture();
}

EditorContext::~EditorContext() {}

void EditorContext::Update() {
    contentBrowserWindow.Update();
    entityListWindow.Update();
    inspectorWindow.Update();
    animEditor.Update();
    textureAssetListWindow.Update();
    fontAssetListWindow.Update();
    tilesheetAssetListWindow.Update();
    soundAssetListWindow.Update();
    animAssetListWindow.Update();
    mtl2dAssetListWindow.Update();
    tilesheetEditor.Update();
    inputTextWindow.Update();

    updateMenubar();
    gameWindow.Update();
}

const nickel::TextCache& EditorContext::FindOrGenFontPrewview(
    nickel::FontHandle handle) {
    if (auto it = fontPreviewTextures_.find(handle);
        it != fontPreviewTextures_.end()) {
        return it->second;
    }

    auto fontMgr = nickel::ECS::Instance().World().res<nickel::FontManager>();
    if (!fontMgr->Has(handle)) {
        return nickel::TextCache::Null;
    }

    auto& font = fontMgr->Get(handle);
    nickel::TextCache cache;
    constexpr std::string_view text = "the brown fox jumps over the lazy dog";
    constexpr int ptSize = 40;
    for (auto c : text) {
        cache.Push(nickel::Character(font.GetGlyph(c, ptSize)));
    }
    return fontPreviewTextures_.emplace(handle, std::move(cache)).first->second;
}

void EditorContext::updateMenubar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("save")) {
                SaveProjectByConfig(EditorContext::Instance().projectInfo,
                                    nickel::ECS::Instance()
                                        .World()
                                        .res<nickel::AssetManager>()
                                        .get());
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            auto addMenuItem = [](const std::string& text, Window& window) {
                bool show = window.IsVisible();
                ImGui::MenuItem(text.c_str(), nullptr, &show);
                window.SetVisible(show);
            };
            addMenuItem("game content window", gameWindow);
            addMenuItem("inspector", EditorContext::Instance().inspectorWindow);
            addMenuItem("entity list",
                        EditorContext::Instance().entityListWindow);
            addMenuItem("content browser", contentBrowserWindow);
            ImGui::MenuItem("imgui demo window", nullptr, &openDemoWindow);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void EditorContext::initGameWindowTexture() {
    auto displayIndex =
        SDL_GetWindowDisplayIndex((SDL_Window*)nickel::ECS::Instance()
                                      .World()
                                      .res<nickel::Window>()
                                      ->Raw());
    SDL_Rect rect;
    SDL_GetDisplayBounds(displayIndex, &rect);
    texture =
        nickel::ECS::Instance()
            .World()
            .res_mut<nickel::TextureManager>()
            ->CreateSolitary(
                nullptr, rect.w, rect.h,
                nickel::rhi::TextureFormat::RGBA8_UNORM,
                nickel::rhi::Flags(nickel::rhi::TextureUsage::TextureBinding) |
                    nickel::rhi::TextureUsage::RenderAttachment);
}
