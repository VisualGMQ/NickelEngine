#pragma once

#include "imgui_plugin.hpp"
#include "nickel.hpp"

class TypeDisplayerRegistrar final
    : public nickel::Singlton<TypeDisplayerRegistrar, false> {
public:
    using FnType = void (*)(mirrow::drefl::any&);

    void Regist(const mirrow::drefl::type* type, FnType fn) {
        fns_.emplace(type, fn);
    }

    void RegistDefaultEnumFn(FnType fn) { defaultEnumFn_ = fn; }
    void RegistDefaultClassFn(FnType fn) { defaultClassFn_ = fn; }
    void RegistDefaultOptionalFn(FnType fn) { defaultOptionalFn_ = fn; }
    void RegistDefaultArrayFn(FnType fn) { defaultArrayFn_ = fn; }

    void Display(mirrow::drefl::any& payload, bool isSameLine = false) const {
        auto typeinfo = payload.type_info();
        if (auto it = fns_.find(typeinfo); it != fns_.end()) {
            if (isSameLine) {
                ImGui::SameLine();
            }
            it->second(payload);
        } else {
            if (isSameLine) {
                ImGui::SameLine();
            }
            if (typeinfo->is_enum()) {
                defaultEnumFn_(payload);
            } else if (typeinfo->is_class()) {
                defaultClassFn_(payload);
            } else if (typeinfo->is_optional()) {
                defaultOptionalFn_(payload);
            } else if (typeinfo->is_array()) {
                defaultArrayFn_(payload);
            }
        }
    }

private:
    std::unordered_map<const mirrow::drefl::type*, FnType> fns_;
    FnType defaultClassFn_;
    FnType defaultEnumFn_;
    FnType defaultOptionalFn_;
    FnType defaultArrayFn_;
};

void DisplayNumeric(mirrow::drefl::any& payload);
void DisplayBoolean(mirrow::drefl::any& payload);
void DisplayString(mirrow::drefl::any& payload);
void DisplayEnum(mirrow::drefl::any& payload);
void DisplayClass(mirrow::drefl::any& payload);
void DisplayOptional(mirrow::drefl::any& payload);
void DisplayArray(mirrow::drefl::any& payload);

void DisplayVec2(mirrow::drefl::any& payload);
void DisplayVec3(mirrow::drefl::any& payload);
void DisplayVec4(mirrow::drefl::any& payload);
void DisplayColor(mirrow::drefl::any& payload);
void DisplayTextureHandle(mirrow::drefl::any& payload);
void DisplayAnimationHandle(mirrow::drefl::any& payload);
void DisplaySoundHandle(mirrow::drefl::any& payload);
void DisplayAnimationPlayer(mirrow::drefl::any& payload);
void DisplaySoundPlayer(mirrow::drefl::any& payload);
void DisplayMaterial2DHandle(mirrow::drefl::any& payload);
void DisplaySpriteMaterial(mirrow::drefl::any& payload);

void RegistDisplayMethods();