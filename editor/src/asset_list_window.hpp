#pragma once

#include "imgui_plugin.hpp"
#include "nickel.hpp"
#include "widget.hpp"

template <typename T>
bool ShowAsset(PopupWindow&, nickel::Handle<T>, const T&, int);

template <>
bool ShowAsset<nickel::Texture>(PopupWindow&, nickel::TextureHandle handle,
                                const nickel::Texture&, int id);

template <>
bool ShowAsset<nickel::Font>(PopupWindow&, nickel::FontHandle handle, const nickel::Font&,
                             int id);

template <typename T>
class AssetListWindow : public PopupWindow {
public:
    using HandleType = nickel::Handle<T>;

    using SelectCallbackFn = std::function<void(HandleType handle)>;

    explicit AssetListWindow(const std::string& title) : PopupWindow(title) {}

    void SetSelectCallback(SelectCallbackFn fn) { fn_ = fn; }

protected:
    void update() override {
        if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_)) {
            int i = 0;
            for (auto& [handle, elem] : gWorld->res<nickel::AssetManager>()
                                            ->SwitchManager<T>()
                                            .AllDatas()) {
                if (ShowAsset(*this, handle, *elem, i)) {
                    if (fn_) {
                        fn_(handle);
                    }
                }
                i++;
            }
            if (ImGui::Button("cancel")) {
                Hide();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

protected:
    SelectCallbackFn fn_;
};

using TextureAssetListWindow = AssetListWindow<nickel::Texture>;
using FontAssetListWindow = AssetListWindow<nickel::Font>;