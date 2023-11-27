#pragma once

#include "mirrow/drefl/value_kind.hpp"
#include "pch.hpp"
#include "imgui.h"
#include "core/singlton.hpp"

class ComponentShowMethods final: public ::nickel::Singlton<ComponentShowMethods, false> {
public:
    using type_info = const ::mirrow::drefl::type*;
    using show_fn = void(*)(type_info, std::string_view name, ::mirrow::drefl::any&, gecs::registry);

    ComponentShowMethods() {
        registDefaultMethods();
    }

    void Regist(type_info type, show_fn show) {
        methods_[type] = show;
    }

    show_fn Find(type_info type);

private:
    std::unordered_map<type_info, show_fn> methods_;

    struct DefaultMethods {
        static void ShowClass(type_info, std::string_view, ::mirrow::drefl::any&, gecs::registry);
        static void ShowNumeric(type_info, std::string_view, ::mirrow::drefl::any&, gecs::registry);
        static void ShowBoolean(type_info, std::string_view, ::mirrow::drefl::any&, gecs::registry);
        static void ShowString(type_info, std::string_view, ::mirrow::drefl::any&, gecs::registry);
        static void ShowEnum(type_info, std::string_view, ::mirrow::drefl::any&, gecs::registry);
        static void ShowOptional(type_info, std::string_view, ::mirrow::drefl::any&, gecs::registry);
    };

    void registDefaultMethods() {
        methods_[::mirrow::drefl::typeinfo<std::string>()] = DefaultMethods::ShowString;
    }
};