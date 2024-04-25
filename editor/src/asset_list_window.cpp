#include "asset_list_window.hpp"
#include "content_browser.hpp"
#include "context.hpp"

template <>
void ShowAsset<nickel::Texture>(const nickel::Texture& texture) {
    if (ImGui::BeginTooltip()) {
        ImGui::Image(texture, {texture.Size().w, texture.Size().h});
        ImGui::EndTooltip();
    }
}

template <>
void ShowAsset<nickel::Tilesheet>(const nickel::Tilesheet& tilesheet) {
    auto& mgr = nickel::ECS::Instance().World().res<nickel::AssetManager>().get();
    if (ImGui::BeginTooltip()) {
        if (mgr.Has(tilesheet.Handle())) {
            auto& texture = mgr.Get(tilesheet.Handle());
            ImGui::Image(texture, {texture.Size().w, texture.Size().h});
        }
        ImGui::EndTooltip();
    }
}