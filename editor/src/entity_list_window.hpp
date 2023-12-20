#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"

struct DragDropInfo {
    gecs::entity targetEnt = gecs::null_entity;
    gecs::entity dragEnt = gecs::null_entity;
    gecs::entity nearestEnt = gecs::null_entity;

    gecs::entity oldDragEnt = gecs::null_entity;
    gecs::entity oldNearestEnt = gecs::null_entity;

    bool dragging = false;
    ImVec2 linePos;

    void Update() {
        oldDragEnt = dragEnt;
        oldNearestEnt = nearestEnt;

        dragEnt = gecs::null_entity;
        nearestEnt = gecs::null_entity;
        targetEnt = gecs::null_entity;
        dragging = false;
    }

    bool CanBeSibling() const {
        return oldNearestEnt != gecs::null_entity &&
               oldDragEnt != gecs::null_entity &&
               oldNearestEnt != oldDragEnt;
    }

    bool IsChangingHierarchy() const {
        return dragEnt != gecs::null_entity && targetEnt != gecs::null_entity;
    }
};

/**
 * @brief [resource][editor]
 */
struct EntityListWindowContext final {
    DragDropInfo dragDropInfo;
};

void EditorEntityListWindow(bool& show, gecs::entity& selected,
                            gecs::registry reg);