#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "show_component.hpp"
#include "spawn_component.hpp"
#include "widget.hpp"

class InspectorWindow: public Window {
protected:
    void update() override;
};

void EditorInspectorWindow(bool& show, gecs::entity entity, gecs::registry reg);