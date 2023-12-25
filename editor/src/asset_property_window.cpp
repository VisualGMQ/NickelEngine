#include "asset_property_window.hpp"
#include "image_view_canva.hpp"
#include "show_component.hpp"


void showWrapper(nickel::gogl::Sampler::Wrapper& wrapper, gecs::registry reg) {
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

void showSampler(nickel::gogl::Sampler& sampler, gecs::registry reg) {
    if (ImGui::TreeNode("sampler")) {
        ShowComponentDefault("filter", sampler.filter);
        showWrapper(sampler.wrapper, reg);
        ImGui::TreePop();
    }
}

bool TexturePropertyPopupWindow(const std::string& title,
                                nickel::TextureHandle handle,
                                AssetPropertyWindowContext& ctx) {
    auto& reg = *gWorld->cur_registry();
    auto& textureMgr = gWorld->res_mut<nickel::AssetManager>()->TextureMgr();

    bool result = false;
    if (ImGui::BeginPopupModal(title.c_str())) {
        if (!textureMgr.Has(handle)) {
            ImGui::Text("invalid texture handle");
            if (ImGui::Button("close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return false;
        }

        auto& texture = textureMgr.Get(handle);

        char buf[512] = {0};
        snprintf(buf, sizeof(buf), "Res://%s",
                 texture.RelativePath().string().c_str());
        ImGui::InputText("filename", buf, sizeof(buf),
                         ImGuiInputTextFlags_ReadOnly);

        float size = ImGui::GetWindowContentRegionMax().x -
                     ImGui::GetStyle().WindowPadding.x * 2.0;
        static nickel::cgmath::Vec2 offset = {0, 0};
        static float scale = 1.0;
        ShowImage({size, size}, offset, scale, handle);

        auto& sampler = ctx.sampler;
        showSampler(sampler, reg);
        ShowComponentDefault("mipmap", sampler.mipmap);

        // show reimport button
        if (sampler != texture.Sampler()) {
            if (ImGui::Button("re-import")) {
                textureMgr.Replace(handle, texture.RelativePath(), sampler);
                result = true;
            }
            ImGui::SameLine();
        }

        // show cancel button
        if (ImGui::Button("cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return result;
}

bool SoundPropertyPopupWindow(const std::string& title,
                              nickel::AudioHandle handle) {
    auto& mgr = gWorld->res_mut<nickel::AssetManager>()->AudioMgr();

    bool result = false;
    if (ImGui::BeginPopupModal(title.c_str())) {
        if (!mgr.Has(handle)) {
            ImGui::Text("invalid audio handle");
            if (ImGui::Button("close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
            return false;
        }

        auto& elem = mgr.Get(handle);

        char buf[512] = {0};
        snprintf(buf, sizeof(buf), "Res://%s",
                 elem.RelativePath().string().c_str());
        ImGui::InputText("filename", buf, sizeof(buf),
                         ImGuiInputTextFlags_ReadOnly);

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
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    return result;
}