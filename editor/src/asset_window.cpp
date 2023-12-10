#include "asset_window.hpp"
#include "content_browser.hpp"


bool ShowTexture(nickel::TextureHandle handle,
                 const nickel::TextureManager::AssetStoreType& texture, int id) {
    bool result = false;
    ImGui::BeginGroup();
    {
        ImGui::PushID(id * 2);
        ImGui::PushID(id * 2 + 1);

        if (ImGui::ImageButton(texture->Raw(), ImVec2{32, 32})) {
            ImGui::CloseCurrentPopup();
            result = true;
        }
        ImGui::Text("%s", texture->RelativePath().string().c_str());

        ImGui::PopID();
        ImGui::PopID();
    }
    ImGui::EndGroup();
    if (ImGui::Button("cancel")) {
        ImGui::CloseCurrentPopup();
    }
    return result;
}

bool ShowFont(nickel::FontHandle handle,
              const nickel::FontManager::AssetStoreType& texture, int id) {
    auto& cbInfo =
        gWorld->cur_registry()->res<gecs::mut<ContentBrowserInfo>>().get();
    bool result = false;
    ImGui::BeginGroup();
    {
        ImGui::PushID(id * 2);
        ImGui::PushID(id * 2 + 1);

        if (ImGui::ImageButton(
                cbInfo
                    .FindTextureOrGen(
                        texture->RelativePath().extension().string())
                    .Raw(),
                ImVec2{32, 32})) {
            ImGui::CloseCurrentPopup();
            result = true;
        }
        ImGui::Text("%s", texture->RelativePath().string().c_str());

        ImGui::PopID();
        ImGui::PopID();
    }
    ImGui::EndGroup();
    if (ImGui::Button("cancel")) {
        ImGui::CloseCurrentPopup();
    }
    return result;
}

nickel::FontHandle FontAssetWindow(const nickel::FontManager& mgr,
                                   const std::string& title) {
    auto& cbInfo =
        gWorld->cur_registry()->res<gecs::mut<ContentBrowserInfo>>().get();
    return AssetWindow<nickel::Font>(mgr, title, ShowFont);
}
