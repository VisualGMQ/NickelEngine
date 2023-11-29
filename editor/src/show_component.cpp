#include "show_component.hpp"
#include "core/assert.hpp"
#include "imgui.h"
#include "mirrow/drefl/value_kind.hpp"

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
    strcpy_s(buf, 1024, str.c_str());
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