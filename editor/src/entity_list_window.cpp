#include "entity_list_window.hpp"

using namespace nickel;

bool beginShowOneEntity(bool isLeaf, gecs::entity& selected,
                        const gecs::entity& ent, const Name& name,
                        DragDropInfo& dragDropOutInfo) {
    auto flag = ImGuiTreeNodeFlags_OpenOnDoubleClick |
                ImGuiTreeNodeFlags_SpanAvailWidth;
    if (isLeaf) {
        flag |= ImGuiTreeNodeFlags_Leaf;
    }
    if (selected == ent) {
        flag |= ImGuiTreeNodeFlags_Selected;
    }
    bool expanded = ImGui::TreeNodeEx((void*)name.name.c_str(), flag, "%s",
                                      name.name.c_str());

    // select event
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        selected = ent;
    }

    // deal drag target
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("entity", &ent, sizeof(gecs::entity));
        dragDropOutInfo.dragEnt = ent;
        dragDropOutInfo.dragging = true;
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload =
                ImGui::AcceptDragDropPayload("entity");
            payload) {
            Assert(payload->DataSize == sizeof(gecs::entity),
                   "drag non-entity in entity list window");
            gecs::entity dragEnt =
                *static_cast<const gecs::entity*>(payload->Data);
            dragDropOutInfo.dragEnt = dragEnt;
            dragDropOutInfo.targetEnt = ent;
        }
        ImGui::EndDragDropTarget();
    }

    // record drag info
    auto cursorPos = ImGui::GetCursorScreenPos();
    auto mousePos = ImGui::GetMousePos();
    auto offsetY = mousePos.y - cursorPos.y;
    if (offsetY < 0 && offsetY >= -3) {
        dragDropOutInfo.nearestEnt = ent;
        dragDropOutInfo.linePos = cursorPos;
    }

    return expanded;
}

void popupMenu(gecs::entity entity) {
    if (ImGui::BeginPopupContextItem(
            std::to_string(gecs::internal::entity_to_integral(entity))
                .c_str())) {
        if (ImGui::Button("delete")) {
            // TODO: not finish
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("make prefabe")) {
            // TODO: not finish
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void endShowOneEntity(const gecs::entity& ent, DragDropInfo& dragDropOutInfo) {
    ImGui::TreePop();
    popupMenu(ent);
}

void showHierarchyEntities(const gecs::entity& entity, gecs::entity& selected,
                           const Name& name, const nickel::Child* children,
                           DragDropInfo& dragDropInfo) {
    if ((children && children->entities.empty()) || !children) {
        if (beginShowOneEntity(true, selected, entity, name, dragDropInfo)) {
            endShowOneEntity(entity, dragDropInfo);
        }
    } else {
        gecs::entity selectedEnt = gecs::null_entity;

        if (beginShowOneEntity(false, selected, entity, name, dragDropInfo)) {
            for (auto ent : children->entities) {
                if (gWorld->cur_registry()->has<nickel::Name>(ent)) {
                    const nickel::Child* children = nullptr;
                    auto& childName =
                        gWorld->cur_registry()->get<nickel::Name>(ent);
                    if (gWorld->cur_registry()->has<Child>(ent)) {
                        children = &gWorld->cur_registry()->get<Child>(ent);
                    }
                    showHierarchyEntities(ent, selected, childName, children,
                                          dragDropInfo);
                }
            }
            endShowOneEntity(entity, dragDropInfo);
        }
    }
}

void EditorEntityListWindow(bool& show, gecs::entity& selected,
                            gecs::registry reg) {
    if (!show) return;

    auto cmds = reg.commands();

    if (ImGui::Begin("Entity List", &show)) {
        if (ImGui::Button("add")) {
            auto ent = cmds.create();
            cmds.emplace<nickel::Name>(
                ent, nickel::Name{"entity-" +
                                  std::to_string(static_cast<uint32_t>(ent))});
        }
        if (reg.alive(selected)) {
            ImGui::SameLine();
            if (ImGui::Button("delete")) {
                cmds.destroy(selected);
            }
        }

        auto hierarchyEntities = gWorld->cur_registry()
                                     ->query<nickel::Child, nickel::Name,
                                             gecs::without<nickel::Parent>>();

        auto ctx = reg.res_mut<EntityListWindowContext>();
        DragDropInfo& dragDropInfo = ctx->dragDropInfo;

        for (auto&& [ent, child, name] : hierarchyEntities) {
            showHierarchyEntities(ent, selected, name, &child, dragDropInfo);
        }

        auto flatEntities =
            gWorld->cur_registry()->query<Name, gecs::without<Parent, Child>>();
        for (auto&& [ent, name] : flatEntities) {
            if (beginShowOneEntity(true, selected, ent, name, dragDropInfo)) {
                endShowOneEntity(ent, dragDropInfo);
            }
        }

        if (dragDropInfo.dragging &&
            dragDropInfo.nearestEnt != gecs::null_entity) {
            ImGui::GetForegroundDrawList()->AddLine(
                dragDropInfo.linePos,
                {ImGui::GetWindowSize().x - dragDropInfo.linePos.x -
                     ImGui::GetStyle().WindowPadding.x,
                 dragDropInfo.linePos.y},
                ImGui::GetColorU32({1, 1, 0, 1}), 1);
        }

        // drag and drop handle
        auto dragEnt = dragDropInfo.dragEnt;
        auto targetEnt = dragDropInfo.targetEnt;
        auto nearestEnt = dragDropInfo.nearestEnt;

        // change hierarchy
        if (dragDropInfo.IsChangingHierarchy()) {
            HierarchyTool tool(targetEnt);
            tool.MoveEntityAsChild(dragEnt);
        } else if (dragDropInfo.CanBeSibling() &&
                   ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            HierarchyTool tool(dragDropInfo.oldNearestEnt);
            tool.MoveAsSibling(dragDropInfo.oldDragEnt);

            dragDropInfo.nearestEnt = gecs::null_entity;
        }

        dragDropInfo.Update();
    }

    ImGui::End();
}
