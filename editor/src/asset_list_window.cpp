#include "asset_list_window.hpp"
#include "content_browser.hpp"
#include "context.hpp"


template <>
bool ShowAsset<nickel::Texture>(
    PopupWindow& window,
    nickel::TextureHandle handle,
    const nickel::Texture& texture, int id) {
    bool result = false;
    ImGui::BeginGroup();
    {
        ImGui::PushID(id * 2);
        ImGui::PushID(id * 2 + 1);

        if (ImGui::ImageButton(texture.Raw(), ImVec2{32, 32})) {
            window.Hide();
            ImGui::CloseCurrentPopup();
            result = true;
        }
        ImGui::Text("%s", texture.RelativePath().string().c_str());

        ImGui::PopID();
        ImGui::PopID();
    }
    ImGui::EndGroup();
    return result;
}

template <>
bool ShowAsset<nickel::Font>(
    PopupWindow& window,
    nickel::FontHandle handle,
    const nickel::Font& texture, int id) {
    auto& cbWindow =
        gWorld->res<gecs::mut<EditorContext>>()->contentBrowserWindow;
    bool result = false;
    ImGui::BeginGroup();
    {
        ImGui::PushID(id * 2);
        ImGui::PushID(id * 2 + 1);

        if (ImGui::ImageButton(
                cbWindow
                    .FindTextureOrGen(
                        texture.RelativePath().extension().string())
                    .Raw(),
                ImVec2{32, 32})) {
            window.Hide();
            ImGui::CloseCurrentPopup();
            result = true;
        }
        ImGui::Text("%s", texture.RelativePath().string().c_str());

        ImGui::PopID();
        ImGui::PopID();
    }
    ImGui::EndGroup();
    return result;
}
