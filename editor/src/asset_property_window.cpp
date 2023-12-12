#include "asset_property_window.hpp"
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
    auto& textureMgr = gWorld->res_mut<nickel::TextureManager>().get();

    bool result = false;
    if (ImGui::BeginPopupModal(title.c_str())) {
        if (!textureMgr.Has(handle)) {
            ImGui::Text("invalid texture handle");
            ImGui::EndPopup();
            return false;
        }

        auto& texture = textureMgr.Get(handle);

        char buf[512] = {0};
        snprintf(buf, sizeof(buf), "Res://%s",
                 texture.RelativePath().string().c_str());
        ImGui::InputText("filename", buf, sizeof(buf),
                         ImGuiInputTextFlags_ReadOnly);

        ImGui::Image(texture.Raw(), ImVec2(texture.Width(), texture.Height()));

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