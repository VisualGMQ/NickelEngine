#include "asset_property_window.hpp"
#include "image_view_canva.hpp"
#include "show_component.hpp"
#include "context.hpp"

void TexturePropertyPopupWindow::showWrapper(
    nickel::gogl::Sampler::Wrapper& wrapper, gecs::registry reg) {
    if (ImGui::TreeNode("wrapper")) {
        // show texture wrapper info
        auto textureWrapperTypeInfo =
            mirrow::drefl::typeinfo<nickel::gogl::TextureWrapperType>();
        if (auto f =
                ComponentShowMethods::Instance().Find(textureWrapperTypeInfo);
            f) {
            auto ref = mirrow::drefl::any_make_ref(wrapper.s);
            f(textureWrapperTypeInfo, "s", ref, reg, {});
            ref = mirrow::drefl::any_make_ref(wrapper.t);
            f(textureWrapperTypeInfo, "t", ref, reg, {});
            // TODO: if texture is 3D, show r component
            // ref = mirrow::drefl::any_make_ref(wrapper.r);
            // f(textureWrapperTypeInfo, "r", ref, reg, {});
        }

        // show border color
        if (wrapper.NeedBorderColor()) {
            ImGui::ColorEdit4("border color", wrapper.borderColor);
        }
        ImGui::TreePop();
    }
}

void TexturePropertyPopupWindow::showSampler(nickel::gogl::Sampler& sampler,
                                             gecs::registry reg) {
    if (ImGui::TreeNode("sampler")) {
        ShowComponentDefault("filter", sampler.filter);
        showWrapper(sampler.wrapper, reg);
        ImGui::TreePop();
    }
}

void TexturePropertyPopupWindow::update() {
    auto& reg = *gWorld->cur_registry();
    auto& textureMgr = gWorld->res_mut<nickel::AssetManager>()->TextureMgr();

    if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_)) {
        if (!textureMgr.Has(handle_)) {
            ImGui::Text("invalid texture handle");
            if (ImGui::Button("close")) {
                Hide();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
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

        showSampler(sampler_, reg);
        ShowComponentDefault("mipmap", sampler_.mipmap);

        // show reimport button
        if (sampler_ != texture.Sampler()) {
            if (ImGui::Button("re-import")) {
                textureMgr.Replace(handle_, texture.RelativePath(), sampler_);
            }
            ImGui::SameLine();
        }

        // show cancel button
        if (ImGui::Button("cancel")) {
            Hide();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void SoundPropertyPopupWindow::update() {
    auto mgr = gWorld->res_mut<nickel::AssetManager>();

    if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        if (!mgr->Has(handle_)) {
            ImGui::Text("invalid audio handle");
            if (ImGui::Button("close")) {
                Hide();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return;
        }

        auto ctx = gWorld->res_mut<EditorContext>();
        auto& elem = ctx->FindOrGenSoundPlayer(handle_);

        // char buf[512] = {0};
        // snprintf(buf, sizeof(buf), "Res://%s",
        //          elem.RelativePath().string().c_str());
        // ImGui::InputText("filename", buf, sizeof(buf),
        //                  ImGuiInputTextFlags_ReadOnly);

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
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void FontPropertyPopupWindow::update() {
    auto& mgr = gWorld->res_mut<nickel::AssetManager>()->FontMgr();

    if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        if (!mgr.Has(handle_)) {
            ImGui::Text("invalid audio handle");
            if (ImGui::Button("close")) {
                Hide();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return;
        }

        auto& elem = mgr.Get(handle_);

        char buf[512] = {0};
        snprintf(buf, sizeof(buf), "Res://%s",
                 elem.RelativePath().string().c_str());
        ImGui::InputText("filename", buf, sizeof(buf),
                         ImGuiInputTextFlags_ReadOnly);

        auto ctx = gWorld->res_mut<EditorContext>();
        auto& preview = ctx->FindOrGenFontPrewview(handle_);
        for (auto& ch : preview.Texts()) {
            ImGui::Image(ch.texture->Raw(),
                         {ch.texture->Size().w, ch.texture->Size().h});
            ImGui::SameLine();
        }

        ImGui::EndPopup();
    }
}