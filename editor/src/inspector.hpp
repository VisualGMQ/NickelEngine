#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "spawn_component.hpp"
#include "asset_property_window.hpp"
#include "widget.hpp"

class ComponentDisplayWidget: public Widget {
public:
    void Update() override;
};

class InspectorWindow : public Window {
public:
    ComponentDisplayWidget cmpDisplayWidget; 
    TexturePropertyWidget texturePropWidget;
    SoundPropertyWidget soundPropWidget;
    FontPropertyWidget fontPropWidget;
    Material2DPropertyWidget mtl2dPropWidget;

    void DisplayComponent() { curWidget_ = &cmpDisplayWidget; }

    void DisplayTextureProperty() { curWidget_ = &texturePropWidget; }

    void DisplaySoundProperty() { curWidget_ = &soundPropWidget; }

    void DisplayFontProperty() { curWidget_ = &fontPropWidget; }

    void DisplayMtl2DProperty() { curWidget_ = &mtl2dPropWidget; }

protected:
    void update() override;

    Widget* curWidget_ = &cmpDisplayWidget;
};

void EditorInspectorWindow(bool& show, gecs::entity entity, gecs::registry reg);