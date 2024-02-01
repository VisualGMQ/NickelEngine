#include "asset_list_window.hpp"
#include "content_browser.hpp"
#include "context.hpp"

template <>
void ShowAsset<nickel::Texture>(nickel::TextureHandle handle,
                                const nickel::Texture& texture) {
    if (ImGui::BeginTooltip()) {
        ImGui::Image(texture.Raw(), {texture.Size().w, texture.Size().h});
        ImGui::EndTooltip();
    }
}

template <>
void ShowAsset<nickel::Font>(nickel::FontHandle handle,
                             const nickel::Font& texture) {}

template <>
void ShowAsset<nickel::Animation>(nickel::AnimationHandle handle,
                                  const nickel::Animation&) {}

template <>
void ShowAsset<nickel::Sound>(nickel::SoundHandle handle,
                              const nickel::Sound& texture) {}

template <>
void ShowAsset<nickel::LuaScript>(nickel::ScriptHandle handle,
                                  const nickel::LuaScript&) {}

template <>
void ShowAsset<nickel::Tilesheet>(nickel::TilesheetHandle handle,
                                  const nickel::Tilesheet& tilesheet) {
    auto& mgr = nickel::ECS::Instance().World().res<nickel::AssetManager>().get();
    if (ImGui::BeginTooltip()) {
        if (mgr.Has(tilesheet.Handle())) {
            auto& texture = mgr.Get(tilesheet.Handle());
            ImGui::Image(texture.Raw(), {texture.Size().w, texture.Size().h});
        }
        ImGui::EndTooltip();
    }
}