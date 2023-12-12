#pragma once

#include "core/singlton.hpp"
#include "imgui.h"
#include "mirrow/drefl/value_kind.hpp"
#include "pch.hpp"


class ComponentShowMethods final
    : public ::nickel::Singlton<ComponentShowMethods, false> {
public:
    using type_info = const ::mirrow::drefl::type*;
    using show_fn = void (*)(type_info, std::string_view name,
                             ::mirrow::drefl::any&, gecs::registry,
                             const std::vector<int>&);

    ComponentShowMethods() { registDefaultMethods(); }

    void Regist(type_info type, show_fn show) { methods_[type] = show; }

    show_fn Find(type_info type);

private:
    std::unordered_map<type_info, show_fn> methods_;

    struct DefaultMethods {
        static void ShowClass(type_info, std::string_view,
                              ::mirrow::drefl::any&, gecs::registry,
                              const std::vector<int>&);
        static void ShowNumeric(type_info, std::string_view,
                                ::mirrow::drefl::any&, gecs::registry,
                                const std::vector<int>&);
        static void ShowBoolean(type_info, std::string_view,
                                ::mirrow::drefl::any&, gecs::registry,
                                const std::vector<int>&);
        static void ShowString(type_info, std::string_view,
                               ::mirrow::drefl::any&, gecs::registry,
                               const std::vector<int>&);
        static void ShowEnum(type_info, std::string_view, ::mirrow::drefl::any&,
                             gecs::registry, const std::vector<int>&);
        static void ShowOptional(type_info, std::string_view,
                                 ::mirrow::drefl::any&, gecs::registry,
                                 const std::vector<int>&);
    };

    void registDefaultMethods() {
        methods_[::mirrow::drefl::typeinfo<std::string>()] =
            DefaultMethods::ShowString;
    }
};

template <typename T>
bool ShowComponentDefault(std::string_view name, T& component) {
    auto filterTypeInfo = mirrow::drefl::typeinfo<T>();
    if (auto f = ComponentShowMethods::Instance().Find(filterTypeInfo); f) {
        auto ref = mirrow::drefl::any_make_ref(component);
        f(filterTypeInfo, name, ref, *gWorld->cur_registry(), {});
        return true;
    }

    return false;
}