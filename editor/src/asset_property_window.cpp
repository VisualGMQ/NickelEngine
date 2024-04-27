#include "asset_property_window.hpp"
#include "context.hpp"
#include "image_view_canva.hpp"
#include "type_displayer.hpp"

void TexturePropertyWidget::Update() {
    auto& reg = *nickel::ECS::Instance().World().cur_registry();
    auto& textureMgr = nickel::ECS::Instance()
                           .World()
                           .res_mut<nickel::AssetManager>()
                           ->TextureMgr();

    if (!textureMgr.Has(handle_)) {
        ImGui::Text("invalid texture handle");
        return;
    }

    auto& texture = textureMgr.Get(handle_);

    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "Res://%s",
             texture.RelativePath().string().c_str());
    ImGui::InputText("filename", buf, sizeof(buf),
                     ImGuiInputTextFlags_ReadOnly);

    float size = ImGui::GetWindowContentRegionMax().x -
                 ImGui::GetStyle().WindowPadding.x * 2.0;
    static nickel::cgmath::Vec2 offset = {0, 0};
    static float scale = 1.0;

    imageViewer_.Resize({size, size});
    imageViewer_.Update();
}

void SoundPropertyWidget::Update() {
    /*
    auto mgr = nickel::ECS::Instance().World().res_mut<nickel::AssetManager>();

    if (!mgr->Has(handle_)) {
        ImGui::Text("invalid audio handle");
        if (ImGui::Button("close")) {
            Hide();
        }
        return;
    }

    auto& ctx = EditorContext::Instance();

    if (elem.IsPlaying()) {
        if (ImGui::Button("pause")) {
            elem.Pause();
        }
    } else {
        if (ImGui::Button("play")) {
            elem.Play();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("rewind")) {
        elem.Rewind();
    }

    float cursor = elem.GetCursor();
    float len = elem.Length();
    std::string progress =
        std::to_string(cursor) + "/" + std::to_string(len) + "s";
    ImGui::SliderFloat(progress.c_str(), &cursor, 0, len, "%.2f");

    bool isLooping = elem.IsLooping();
    ImGui::Checkbox("looping", &isLooping);
    if (isLooping != elem.IsLooping()) {
        elem.SetLoop(isLooping);
    }

    // show cancel button
    if (ImGui::Button("cancel")) {
        elem.Stop();
        Hide();
    }
    */
}

void FontPropertyWidget::Update() {
    auto& mgr = nickel::ECS::Instance()
                    .World()
                    .res_mut<nickel::AssetManager>()
                    ->FontMgr();

    if (!mgr.Has(handle_)) {
        ImGui::Text("invalid audio handle");
        return;
    }

    auto& elem = mgr.Get(handle_);

    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "Res://%s",
             elem.RelativePath().string().c_str());
    ImGui::InputText("filename", buf, sizeof(buf),
                     ImGuiInputTextFlags_ReadOnly);

    auto& ctx = EditorContext::Instance();
    auto& preview = ctx.FindOrGenFontPrewview(handle_);
    for (auto& ch : preview.Texts()) {
        ImGui::Image(*ch.texture, {ch.texture->Size().w, ch.texture->Size().h});
        ImGui::SameLine();
    }
}

void Material2DPropertyWidget::Update() {
    auto mgr =
        nickel::ECS::Instance().World().res_mut<nickel::Material2DManager>();
    if (!mgr->Has(handle_)) {
        ImGui::Text("invalid material handle");
        return;
    }

    auto& elem = mgr->Get(handle_);

    if (ImGui::CollapsingHeader("texture")) {
        auto textureMgr =
            nickel::ECS::Instance().World().res_mut<nickel::TextureManager>();
        auto handle = elem.GetTexture();

        char str[MAX_PATH_LENGTH] = "no texture";
        if (textureMgr->Has(handle)) {
            static ImageViewCanva canva;
            canva.ChangeTexture(handle);
            canva.Update();

            strcpy(str,
                   textureMgr->Get(handle).RelativePath().string().c_str());
        }

        ImGui::Text("path");
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::InputText("###mtl2dpropwindow-texture", str, sizeof(str));
        ImGui::EndDisabled();
        if (ImGui::Button("change")) {
            auto& window = EditorContext::Instance().textureAssetListWindow;
            window.Show();
            window.SetSelectCallback([&elem](nickel::TextureHandle handle) {
                elem.ChangeTexture(handle);
            });
        }
    }

    if (ImGui::CollapsingHeader("sampler")) {
        auto desc = elem.GetSamplerDesc();

        auto uPayload = mirrow::drefl::any_make_ref(desc.u);
        ImGui::Text("u");
        ImGui::SameLine();
        DisplayEnum(uPayload);

        auto vPayload = mirrow::drefl::any_make_ref(desc.v);
        ImGui::Text("v");
        ImGui::SameLine();
        DisplayEnum(vPayload);

        auto minPayload = mirrow::drefl::any_make_ref(desc.min);
        ImGui::Text("min");
        ImGui::SameLine();
        DisplayEnum(minPayload);

        auto magPayload = mirrow::drefl::any_make_ref(desc.mag);
        ImGui::Text("mag");
        ImGui::SameLine();
        DisplayEnum(magPayload);

        if (desc != elem.GetSamplerDesc()) {
            elem.ChangeSampler(desc.u, desc.v, desc.min, desc.mag);
        }
    }
}

void DisplayTransform(mirrow::drefl::any& payload);