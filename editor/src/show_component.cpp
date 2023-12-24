#include "show_component.hpp"
#include "core/assert.hpp"
#include "imgui.h"
#include "asset_list_window.hpp"

using namespace nickel;

ComponentShowMethods::show_fn ComponentShowMethods::Find(type_info type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second;
    }

    if (type->is_numeric()) {
        return DefaultMethods::ShowNumeric;
    }

    if (type->is_boolean()) {
        return DefaultMethods::ShowBoolean;
    }

    if (type->is_class()) {
        return DefaultMethods::ShowClass;
    }

    if (type->is_enum()) {
        return DefaultMethods::ShowEnum;
    }

    if (type->is_optional()) {
        return DefaultMethods::ShowOptional;
    }

    return nullptr;
}

void ComponentShowMethods::DefaultMethods::ShowClass(
    const mirrow::drefl::type* typeInfo, std::string_view name,
    ::mirrow::drefl::any& value, gecs::registry reg, const std::vector<int>&) {
    Assert(typeInfo->is_class(), "type incorrect");

    if (ImGui::TreeNode(name.data())) {
        auto classInfo = typeInfo->as_class();
        auto id = 0;
        for (auto&& var : classInfo->properties()) {
            auto varType = var->type_info();
            auto showMethod = ComponentShowMethods::Instance().Find(varType);
            ImGui::PushID(id);
            if (showMethod) {
                auto ref = var->call(value);
                showMethod(varType, var->name(), ref, reg, var->attributes());
            }
            ImGui::PopID();
            id ++;
        }

        ImGui::TreePop();
    }
}

void ComponentShowMethods::DefaultMethods::ShowNumeric(
    type_info type, std::string_view name, ::mirrow::drefl::any& value,
    gecs::registry, const std::vector<int>&) {
    Assert(type->is_numeric(), "type incorrect");
    auto numeric = type->as_numeric();

    if (numeric->is_integer()) {
        int i = numeric->get_value(value);
        ImGui::DragInt(name.data(), &i);
        numeric->set_value(value, (long)i);
    } else {
        float f = numeric->get_value(value);
        ImGui::DragFloat(name.data(), &f);
        numeric->set_value(value, f);
    }
}

void ComponentShowMethods::DefaultMethods::ShowBoolean(
    type_info type, std::string_view name, ::mirrow::drefl::any& value,
    gecs::registry, const std::vector<int>&) {
    Assert(type->is_boolean(), "type incorrect");
    auto boolean = type->as_boolean();

    bool b = boolean->get_value(value);
    ImGui::Checkbox(name.data(), &b);
    boolean->set_value(value, b);
}

void ComponentShowMethods::DefaultMethods::ShowString(
    type_info type, std::string_view name, ::mirrow::drefl::any& value,
    gecs::registry, const std::vector<int>&) {
    Assert(type->is_string(), "type incorrect");

    auto string_type = type->as_string();
    auto str = string_type->get_str(value);

    char buf[1024] = {0};
    strcpy(buf, str.c_str());
    ImGui::InputText(name.data(), buf, sizeof(buf));

    if (buf != str) {
        string_type->set_value(value, std::string(buf));
    }
}

void ComponentShowMethods::DefaultMethods::ShowEnum(
    type_info type, std::string_view name, ::mirrow::drefl::any& value,
    gecs::registry, const std::vector<int>&) {
    Assert(type->is_enum(), "type incorrect");

    auto enum_info = type->as_enum();

    static std::vector<const char*> enumNames;
    enumNames.clear();

    int curItem = enum_info->get_value(value);
    int idx = 0;
    auto& enums = enum_info->enums();

    for (int i = 0; i < enums.size(); i++) {
        auto& e = enums[i];

        enumNames.push_back(e.name().c_str());

        if (e.value() == curItem) {
            idx = i;
        }
    }

    ImGui::Combo(name.data(), &idx, enumNames.data(), enumNames.size());

    enum_info->set_value(value, enums[idx].value());
}

void ComponentShowMethods::DefaultMethods::ShowOptional(
    type_info type, std::string_view name, ::mirrow::drefl::any& value,
    gecs::registry reg, const std::vector<int>&) {
    Assert(type->is_optional(), "type incorrect");

    auto& optional_type = *type->as_optional();
    if (optional_type.has_value(value)) {
        auto elem = optional_type.get_value(value);
        auto show = ComponentShowMethods::Instance().Find(elem.type_info());
        if (show) {
            show(elem.type_info(), name, elem, reg, type->attributes());
        }
    } else {
        if (ImGui::TreeNode(name.data())) {
            ImGui::Text("none");
            ImGui::TreePop();
        }
    }
}

void ShowVec2(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry,
              const std::vector<int>&) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec2>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec2>(value);
    ImGui::DragFloat2(name.data(), vec->data);
}

void ShowVec3(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry,
              const std::vector<int>&) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec3>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec3>(value);
    ImGui::DragFloat3(name.data(), vec->data);
}

void ShowVec4(const mirrow::drefl::type* type, std::string_view name,
              mirrow::drefl::any& value, gecs::registry,
              const std::vector<int>& attrs) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<cgmath::Vec4>(),
        "type incorrect");

    auto vec = mirrow::drefl::try_cast<cgmath::Vec4>(value);
    if (std::find(attrs.begin(), attrs.end(), AttrRange01) != attrs.end()) {
        ImGui::DragFloat4(name.data(), vec->data, 0.01, 0, 1);
    } else if (std::find(attrs.begin(), attrs.end(), AttrColor) !=
               attrs.end()) {
        ImGui::ColorEdit4(name.data(), vec->data);
    } else {
        ImGui::DragFloat4(name.data(), vec->data);
    }
}

template <typename MgrType>
nickel::Handle<typename MgrType::AssetType> SelectAndChangeAsset(
    MgrType& mgr, std::string_view buttonText, const std::string& title) {
    using AssetType = typename MgrType::AssetType;
    if (ImGui::Button(buttonText.data())) {
        ImGui::OpenPopup(title.c_str());
    }
    if constexpr (std::is_same_v<AssetType, nickel::Texture>) {
        return TextureAssetWindow(mgr, title);
    } else if constexpr (std::is_same_v<AssetType, nickel::Font>) {
        return FontAssetWindow(mgr, title);
    }
    return {};
}

void ShowTextureHandle(const mirrow::drefl::type* type, std::string_view name,
                       mirrow::drefl::any& value, gecs::registry reg,
                       const std::vector<int>&) {
    Assert(
        type->is_class() && type == ::mirrow::drefl::typeinfo<TextureHandle>(),
        "type incorrect");

    auto& handle = *mirrow::drefl::try_cast<TextureHandle>(value);
    auto mgr = reg.res<AssetManager>();
    char buf[1024] = {0};
    if (mgr->Has(handle)) {
        auto& texture = mgr->Get(handle);
        std::filesystem::path texturePath = texture.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", texturePath.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no texture");
    }

    static std::string title = "asset texture";
    auto newHandle = SelectAndChangeAsset(
        reg.res_mut<AssetManager>()->TextureMgr(), buf, title);
    if (newHandle) {
        handle = newHandle;
    }

    if (handle && ImGui::TreeNode("thumbnail")) {
        auto& texture = mgr->Get(handle);
        ImGui::Image(texture.Raw(), ImVec2(texture.Width(), texture.Height()));
        ImGui::TreePop();
    }
}

void ShowFontHandle(const mirrow::drefl::type* type, std::string_view name,
                    mirrow::drefl::any& value, gecs::registry reg,
                    const std::vector<int>&) {
    Assert(type->is_class() && type == ::mirrow::drefl::typeinfo<FontHandle>(),
           "type incorrect");

    auto& handle = *mirrow::drefl::try_cast<FontHandle>(value);
    auto& mgr = reg.res<AssetManager>()->FontMgr();
    char buf[1024] = {0};
    if (mgr.Has(handle)) {
        auto& font = mgr.Get(handle);
        std::filesystem::path path = font.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", path.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no font");
    }

    static std::string title = "asset font";
    auto newHandle = SelectAndChangeAsset(mgr, buf, title);
    if (newHandle) {
        handle = newHandle;
    }
}

void ShowAnimationPlayer(const mirrow::drefl::type* type, std::string_view name,
                         mirrow::drefl::any& value, gecs::registry reg,
                         const std::vector<int>&) {
    Assert(type->is_class() &&
               type == ::mirrow::drefl::typeinfo<AnimationPlayer>(),
           "type incorrect");

    AnimationPlayer& player = *mirrow::drefl::try_cast<AnimationPlayer>(value);
    auto handle = player.GetAnim();
    auto& mgr = reg.res<AnimationManager>().get();
    static char buf[1024] = {0};
    if (handle && mgr.Has(handle)) {
        // snprintf(buf, sizeof(buf), "%s", mgr.Get(handle).RelativePath());
    } else {
        snprintf(buf, sizeof(buf), "%s", "no animation");
    }

    int selectedItem = -1;
    std::array<const char*, 2> items = {"create new", "load"};
    ImGui::Combo(buf, &selectedItem, items.data(), items.size(),
                 ImGuiComboFlags_NoPreview);
    if (selectedItem == 0) {
        // TODO: create new animation and switch to animation panel
    } else if (selectedItem == 1) {
        // TODO: load from disk
    }
}

void ShowLabel(const mirrow::drefl::type* type, std::string_view name,
               mirrow::drefl::any& value, gecs::registry reg,
               const std::vector<int>&) {
    Assert(type->is_class() && type == ::mirrow::drefl::typeinfo<ui::Label>(),
           "type incorrect");

    ui::Label& label = *mirrow::drefl::try_cast<ui::Label>(value);

    static char buf[1024] = {0};
    auto text = label.GetText().to_string();
    std::strcpy(buf, text.c_str());

    ImGui::InputTextMultiline("text", buf, sizeof(buf));

    if (buf != text) {
        utf8string str{buf};
        label.SetText(str);
    }

    ImGui::ColorEdit4("color", label.color.data);
    ImGui::ColorEdit4("pressing color", label.pressColor.data);
    ImGui::ColorEdit4("hovering color", label.hoverColor.data);

    int size = label.GetPtSize();
    ImGui::DragInt("pt", &size, 1.0, 1);
    if (size != label.GetPtSize()) {
        label.SetPtSize(size);
    }

    // show font
    auto fontHandle = label.GetFont();
    auto ref = mirrow::drefl::any_make_ref(fontHandle);
    ShowFontHandle(ref.type_info(), "font", ref, reg, {});
    label.ChangeFont(fontHandle);
}
