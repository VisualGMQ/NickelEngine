#pragma once

#include "pch.hpp"
#include "imgui.h"
#include "core/singlton.hpp"

class ComponentShowMethods final: public ::nickel::Singlton<ComponentShowMethods, false> {
public:
    using type_info = ::mirrow::drefl::type_info;
    using show_fn = void(*)(type_info, std::string_view name, ::mirrow::drefl::basic_any&);

    ComponentShowMethods() {
        registDefaultMethods();
    }

    void Regist(type_info type, show_fn&& show) {
        methods_[type] = show;
    }

    show_fn Find(type_info type);

private:
    std::unordered_map<::mirrow::drefl::type_info, show_fn> methods_;

    struct DefaultMethods {
        static void ShowClass(type_info, std::string_view, ::mirrow::drefl::basic_any&);
        static void ShowNumeric(type_info, std::string_view, ::mirrow::drefl::basic_any&);
        static void ShowBoolean(type_info, std::string_view, ::mirrow::drefl::basic_any&);
        static void ShowString(type_info, std::string_view, ::mirrow::drefl::basic_any&);
        static void ShowEnum(type_info, std::string_view, ::mirrow::drefl::basic_any&);
    };

    void registDefaultMethods() {
        methods_[::mirrow::drefl::reflected_type<std::string>()] = DefaultMethods::ShowString;
    }
};