#include "misc/hierarchy.hpp"

namespace nickel {

void doUpdateGlobalTransform(gecs::entity entity, Transform& trans,
                             GlobalTransform& globalTrans,
                             const Child& children, gecs::registry reg) {
    for (auto ent : children.entities) {
        if (!(reg.has<Transform>(ent) && reg.has<GlobalTransform>(ent))) {
            continue;
        }
        auto& childTrans = reg.get_mut<Transform>(ent);
        auto& childGlobalTrans = reg.get_mut<GlobalTransform>(ent);
        childGlobalTrans.mat = globalTrans.mat * childTrans.ToMat();

        if (reg.has<Child>(ent)) {
            doUpdateGlobalTransform(ent, childTrans, childGlobalTrans,
                                    reg.get<Child>(ent), reg);
        }
    }
}

void UpdateGlobalTransform(
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>, Child,
                  gecs::without<Parent>>
        root,
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>,
                  gecs::without<Parent>>
        querier,
    gecs::registry reg) {
    for (auto&& [entity, gTrans, trans] : querier) {
        gTrans.mat = trans.ToMat();
    }

    for (auto&& [entity, gTrans, trans, child] : root) {
        doUpdateGlobalTransform(entity, trans, gTrans, child, reg);
    }
}

void HierarchyTool::MoveEntityAsChild(
    gecs::entity other, std::optional<size_t> idx) {
    gecs::registry reg(*gWorld->cur_registry());

    HierarchyTool tool(other);
    auto& parent = tool.disconnectParentOrCreate(reg);
    parent.entity = ent_;

    Child* child = nullptr;
    if (!reg.has<Child>(ent_)) {
        child = &reg.commands().emplace<Child>(ent_);
    } else {
        child = &reg.get_mut<Child>(ent_);
    }
    if (idx) {
        child->entities.insert(child->entities.begin() + idx.value(), other);
    } else {
        child->entities.push_back(other);
    }
}

void HierarchyTool::ChangeOrder(gecs::entity childEnt, size_t idx) const {
    gecs::registry reg(*gWorld->cur_registry());
    if (!reg.has<Child>(ent_)) {
        return;
    }
    auto& children = reg.get_mut<Child>(ent_).entities;
    int i = 0;
    for (; i < children.size(); i++) {
        if (children[i] == childEnt) {
            break;
        }
    }
    if (i >= children.size()) {
        return;
    }

    i = i < idx ? idx : idx - 1;

    children.insert(children.begin() + i, childEnt);
}

void HierarchyTool::MoveAsSibling(gecs::entity entity) const {
    gecs::registry reg(*gWorld->cur_registry());
    if (!HasHierarchy() || IsRoot()) {
        if (reg.has<Parent>(entity)) {
            HierarchyTool tool(entity);
            tool.disconnectParent(reg);
            reg.commands().remove<Parent>(entity);
        }
        return;
    }

    auto& parent = reg.get_mut<Parent>(ent_);

    HierarchyTool tool(entity);
    auto& childParent = tool.disconnectParentOrCreate(reg);
    childParent.entity = parent.entity;

    auto& children = reg.get_mut<Child>(parent.entity).entities;
    auto it = std::find(children.begin(), children.end(), ent_);
    if (it == children.end()) {
        return;
    }

    children.insert(it + 1, entity);
}

Parent& HierarchyTool::disconnectParentOrCreate(gecs::registry reg) const {
    if (!HasHierarchy() || IsRoot(ent_)) {
        return reg.commands().emplace<Parent>(ent_);
    }

    auto& parent = reg.get_mut<Parent>(ent_);
    auto& parentChildren = reg.get_mut<Child>(parent.entity).entities;
    parentChildren.erase(
        std::remove(parentChildren.begin(), parentChildren.end(), ent_),
        parentChildren.end());
    parent.entity = gecs::null_entity;
    return parent;
}

Parent* HierarchyTool::disconnectParent(gecs::registry reg) const {
    if (IsRoot(ent_)) {
        return nullptr;
    }

    auto parent = &reg.get_mut<Parent>(ent_);
    auto& parentChildren = reg.get_mut<Child>(parent->entity).entities;
    parentChildren.erase(
        std::remove(parentChildren.begin(), parentChildren.end(), ent_),
        parentChildren.end());
    parent->entity = gecs::null_entity;
    return parent;
}

}  // namespace nickel