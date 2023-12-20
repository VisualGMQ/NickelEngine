#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "show_component.hpp"
#include "spawn_component.hpp"

void EditorInspectorWindow(bool& show, gecs::entity entity, gecs::registry reg);