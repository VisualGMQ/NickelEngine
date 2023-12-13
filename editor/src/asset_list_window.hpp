#pragma once

#include "imgui_plugin.hpp"
#include "nickel.hpp"

template <typename T>
using AssetShowFn = std::function<bool(
    nickel::Handle<T>, const typename nickel::Manager<T>::AssetStoreType&,
    int)>;

bool ShowTexture(nickel::TextureHandle handle,
                 const nickel::TextureManager::AssetStoreType& texture, int id);

bool ShowFont(nickel::FontHandle handle,
              const nickel::FontManager::AssetStoreType& texture, int id);

template <typename T>
nickel::Handle<T> AssetWindow(const nickel::Manager<T>& mgr,
                              const std::string& title,
                              const AssetShowFn<T>& showFn) {
    nickel::Handle<T> selectHandle;
    if (ImGui::BeginPopupModal(title.c_str())) {
        int i = 0;
        for (auto& [handle, elem] : mgr.AllDatas()) {
            if (showFn(handle, elem, i)) {
                selectHandle = handle;
            }
            i++;
        }
        if (ImGui::Button("cancel")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    return selectHandle;
}

inline nickel::TextureHandle TextureAssetWindow(
    const nickel::TextureManager& mgr, const std::string& title) {
    return AssetWindow<nickel::Texture>(mgr, title, ShowTexture);
}

nickel::FontHandle FontAssetWindow(const nickel::FontManager& mgr,
                                          const std::string& title);
