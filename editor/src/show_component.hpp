#pragma once

#include "nickel.hpp"

class ComponentShowMethods final
    : public ::nickel::Singlton<ComponentShowMethods, false> {
public:
    using type_info = const ::mirrow::drefl::type*;
    using show_fn =
        std::function<void(type_info, std::string_view name,
                           ::mirrow::drefl::any&, gecs::registry)>;

    void Regist(type_info type, show_fn show) { methods_[type] = show; }

    show_fn Find(type_info type);

private:
    std::unordered_map<type_info, show_fn> methods_;

    struct DefaultMethods {
        static void ShowClass(type_info parent, std::string_view,
                              ::mirrow::drefl::any&, gecs::registry);
        static void ShowNumeric(type_info parent, std::string_view,
                                ::mirrow::drefl::any&, gecs::registry);
        static void ShowBoolean(type_info parent, std::string_view,
                                ::mirrow::drefl::any&, gecs::registry);
        static void ShowString(type_info parent, std::string_view,
                               ::mirrow::drefl::any&, gecs::registry);
        static void ShowEnum(type_info parent, std::string_view,
                             ::mirrow::drefl::any&, gecs::registry);
        static void ShowOptional(type_info parent, std::string_view,
                                 ::mirrow::drefl::any&, gecs::registry);
    };
};

inline bool DisplayComponent(const mirrow::drefl::type* parent,
                             std::string_view name, ::mirrow::drefl::any& obj,
                             gecs::registry reg) {
    if (auto fn = ComponentShowMethods::Instance().Find(obj.type_info()); fn) {
        fn(parent, name, obj, reg);
        return true;
    }
    return false;
}

template <typename T>
bool DisplayComponent(std::string_view name, T& component) {
    auto type = mirrow::drefl::typeinfo<T>();
    if (auto f = ComponentShowMethods::Instance().Find(type); f) {
        auto ref = mirrow::drefl::any_make_ref(component);
        f(type, name, ref, *nickel::ECS::Instance().World().cur_registry());
        return true;
    }

    return false;
}

void DisplayVec2(const mirrow::drefl::type* parent, std::string_view name,
                 mirrow::drefl::any& value, gecs::registry);

void DisplayVec3(const mirrow::drefl::type* parent, std::string_view name,
                 mirrow::drefl::any& value, gecs::registry);

void DisplayVec4(const mirrow::drefl::type* parent, std::string_view name,
                 mirrow::drefl::any& value, gecs::registry);

void DisplaySprite(const mirrow::drefl::type* parent, std::string_view name,
                   mirrow::drefl::any& value, gecs::registry reg);

void DisplayTextureHandle(const mirrow::drefl::type* parent,
                          std::string_view name, mirrow::drefl::any& value,
                          gecs::registry reg);

void DisplayMaterial2DHandle(const mirrow::drefl::type* parent,
                             std::string_view name, mirrow::drefl::any& value,
                             gecs::registry reg);

void DisplayAnimationPlayer(const mirrow::drefl::type* parent,
                            std::string_view name, mirrow::drefl::any& value,
                            gecs::registry reg);

void DisplaySoundPlayer(const mirrow::drefl::type* parent,
                        std::string_view name, mirrow::drefl::any& value,
                        gecs::registry reg);

// void DisplayLabel(const mirrow::drefl::type* parent, std::string_view name,
//                   mirrow::drefl::any& value, gecs::registry reg);
