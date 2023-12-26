#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "widget.hpp"

struct DragDropInfo final {
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

class EntityListWindow final: public Window {
public:
    void Update() override;

    void Show() { show_ = true; }
    void Hide() { show_ = false; }
    bool IsVisiable() const { return show_; }

    auto GetSelected() const { return selected_; }

private:
    DragDropInfo dragDropInfo_;
    gecs::entity selected_ = gecs::null_entity;

    bool beginShowOneEntity(bool isLeaf, gecs::entity& selected,
                            const gecs::entity& ent, const nickel::Name& name,
                            DragDropInfo& dragDropOutInfo);
    void endShowOneEntity(const gecs::entity& ent, DragDropInfo& dragDropOutInfo);

    void showHierarchyEntities(const gecs::entity& entity,
                               gecs::entity& selected, const nickel::Name& name,
                               const nickel::Child* children,
                               DragDropInfo& dragDropInfo);

    void popupMenu(gecs::entity entity);
};
