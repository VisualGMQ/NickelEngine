#include "entity_list_window.hpp"
#include "context.hpp"

using namespace nickel;

bool EntityListWindow::beginShowOneEntity(bool isLeaf, gecs::entity& selected,
                                          const gecs::entity& ent,
                                          const Name& name,
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
        auto reg = gWorld->cur_registry();
        auto ctx = gWorld->res_mut<EditorContext>();
        if (reg->has<AnimationPlayer>(ent)) {
            auto& player = reg->get<AnimationPlayer>(ent);
            ctx->animEditor.ChangePlayer(ent, player.Anim());
        } else {
            ctx->animEditor.ChangePlayer(gecs::null_entity, AnimationHandle::Null());
        }
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

void EntityListWindow::popupMenu(gecs::entity entity) {
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

void EntityListWindow::endShowOneEntity(const gecs::entity& ent,
                                        DragDropInfo& dragDropOutInfo) {
    ImGui::TreePop();
    popupMenu(ent);
}

void EntityListWindow::showHierarchyEntities(const gecs::entity& entity,
                                             gecs::entity& selected,
                                             const Name& name,
                                             const nickel::Child* children,
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

void EntityListWindow::Update() {
    if (!IsVisible()) return;

    auto reg = gWorld->cur_registry();
    auto cmds = reg->commands();

    if (ImGui::Begin("Entity List", &show_)) {
        if (ImGui::Button("add")) {
            auto ent = cmds.create();
            cmds.emplace<nickel::Name>(
                ent, nickel::Name{"entity-" +
                                  std::to_string(static_cast<uint32_t>(ent))});
        }
        if (reg->alive(selected_)) {
            ImGui::SameLine();
            if (ImGui::Button("delete")) {
                cmds.destroy(selected_);
            }
        }

        auto hierarchyEntities = reg->query<nickel::Child, nickel::Name,
                                             gecs::without<nickel::Parent>>();

        for (auto&& [ent, child, name] : hierarchyEntities) {
            showHierarchyEntities(ent, selected_, name, &child, dragDropInfo_);
        }

        auto flatEntities =
            gWorld->cur_registry()->query<Name, gecs::without<Parent, Child>>();
        for (auto&& [ent, name] : flatEntities) {
            if (beginShowOneEntity(true, selected_, ent, name, dragDropInfo_)) {
                endShowOneEntity(ent, dragDropInfo_);
            }
        }

        if (dragDropInfo_.dragging &&
            dragDropInfo_.nearestEnt != gecs::null_entity) {
            ImGui::GetForegroundDrawList()->AddLine(
                dragDropInfo_.linePos,
                {ImGui::GetWindowSize().x - dragDropInfo_.linePos.x -
                     ImGui::GetStyle().WindowPadding.x,
                 dragDropInfo_.linePos.y},
                ImGui::GetColorU32({1, 1, 0, 1}), 1);
        }

        // drag and drop handle
        auto dragEnt = dragDropInfo_.dragEnt;
        auto targetEnt = dragDropInfo_.targetEnt;
        auto nearestEnt = dragDropInfo_.nearestEnt;

        // change hierarchy
        if (dragDropInfo_.IsChangingHierarchy()) {
            HierarchyTool tool(targetEnt);
            tool.MoveEntityAsChild(dragEnt);
        } else if (dragDropInfo_.CanBeSibling() &&
                   ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            HierarchyTool tool(dragDropInfo_.oldNearestEnt);
            tool.MoveAsSibling(dragDropInfo_.oldDragEnt);

            dragDropInfo_.nearestEnt = gecs::null_entity;
        }

        dragDropInfo_.Update();
    }

    ImGui::End();

    if (!reg->alive(selected_)) {
        selected_ = gecs::null_entity;
    }
}
