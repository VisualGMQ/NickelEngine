#include "show_component.hpp"

ComponentShowMethods::show_fn ComponentShowMethods::Find(type_info type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second;
    }

    if (type.is_fundamental()) {
        auto fund = type.as_fundamental();
        if (fund.is_boolean()) {
            return DefaultMethods::ShowBoolean;
        } else {
            return DefaultMethods::ShowNumeric;
        }
    }

    if (type.is_class()) {
        return DefaultMethods::ShowClass;
    }

    if (type.is_enum()) {
        return DefaultMethods::ShowEnum;
    }

    return nullptr;
}

void ComponentShowMethods::DefaultMethods::ShowClass(
    type_info typeInfo, std::string_view name,
    ::mirrow::drefl::basic_any& value, gecs::registry reg) {
    Assert(typeInfo.is_class(), "type incorrect");

    if (ImGui::TreeNode(name.data())) {
        auto classInfo = typeInfo.as_class();
        for (auto&& var : classInfo.vars()) {
            auto varType = type_info{var.raw_type()};
            auto showMethod = ComponentShowMethods::Instance().Find(varType);
            if (showMethod) {
                auto ref =
                    ::mirrow::drefl::invoke_by_any_return_ref(var, &value);
                showMethod(varType, var.name(), ref, reg);
            }
        }

        ImGui::TreePop();
    }
}

void ComponentShowMethods::DefaultMethods::ShowNumeric(
    type_info type, std::string_view name, ::mirrow::drefl::basic_any& value,
    gecs::registry) {
    Assert(type.is_fundamental(), "type incorrect");
    auto fund = type.as_fundamental();
    Assert(fund.is_floating_point() || fund.is_integer(), "type incorrect");

    if (fund.is_integer()) {
        if (fund.is_signed()) {
            int data = value.try_cast_integral().value();
            ImGui::InputInt(name.data(), &data);
            value.deep_set(::mirrow::drefl::reference_any{data});
        } else {
            int data = value.try_cast_uintegral().value();
            ImGui::InputInt(name.data(), &data);
            value.deep_set(::mirrow::drefl::reference_any{std::max(data, 0)});
        }
    } else {
        double data = value.try_cast_floating_point().value();
        ImGui::InputDouble(name.data(), &data);
        value.deep_set(::mirrow::drefl::reference_any{data});
    }
}

void ComponentShowMethods::DefaultMethods::ShowBoolean(
    type_info type, std::string_view name, ::mirrow::drefl::basic_any& value,
    gecs::registry) {
    Assert(type.is_fundamental() && type.as_fundamental().is_boolean(),
           "type incorrect");
    auto fund = type.as_fundamental();

    bool& data = value.cast<bool>();

    const char* labels[] = {"false", "true"};
    int curItem = data;
    ImGui::Combo(name.data(), &curItem, labels, 2);
    data = static_cast<bool>(curItem);
}

void ComponentShowMethods::DefaultMethods::ShowString(
    type_info type, std::string_view name, ::mirrow::drefl::basic_any& value,
    gecs::registry) {
    Assert(type.is_class() && type.as_class().is_string(), "type incorrect");

    std::string& data = value.cast<std::string>();

    char buf[1024] = {0};
    ImGui::InputText(name.data(), buf, sizeof(buf));
    data = buf;
}

void ComponentShowMethods::DefaultMethods::ShowEnum(
    type_info type, std::string_view name, ::mirrow::drefl::basic_any& value,
    gecs::registry) {
    Assert(type.is_enum(), "type incorrect");
    // TODO: support enum

    int curItem = 0;
    const char* labels[] = {"currently enum not support"};
    ImGui::Combo(name.data(), &curItem, labels, 1);
}