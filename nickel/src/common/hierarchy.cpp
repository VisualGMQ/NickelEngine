#include "common/hierarchy.hpp"
#include "common/profile.hpp"

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
    PROFILE_BEGIN();

    for (auto&& [entity, gTrans, trans] : querier) {
        gTrans.mat = trans.ToMat();
    }

    for (auto&& [entity, gTrans, trans, child] : root) {
        doUpdateGlobalTransform(entity, trans, gTrans, child, reg);
    }
}

void HierarchyTool::MoveEntityAsChild(gecs::entity other,
                                      std::optional<size_t> idx) {
    HierarchyTool tool(reg_, other);
    auto& parent = tool.disconnectParentOrCreate(reg_);
    parent.entity = ent_;

    Child* child = nullptr;
    if (!reg_.has<Child>(ent_)) {
        child = &reg_.commands().emplace<Child>(ent_);
    } else {
        child = &reg_.get_mut<Child>(ent_);
    }
    if (idx) {
        child->entities.insert(child->entities.begin() + idx.value(), other);
    } else {
        child->entities.push_back(other);
    }
}


void HierarchyTool::PreorderVisit(const PreorderVisitFn& fn) {
    doPreorderVisit(ent_, fn, reg_);
}

void HierarchyTool::ChangeOrder(gecs::entity childEnt, size_t idx) {
    if (!reg_.has<Child>(ent_)) {
        return;
    }

    auto& children = reg_.get_mut<Child>(ent_).entities;
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

void HierarchyTool::MoveAsSibling(gecs::entity entity) {
    if (!HasHierarchy() || IsRoot()) {
        if (reg_.has<Parent>(entity)) {
            HierarchyTool tool(reg_, entity);
            tool.disconnectParent(reg_);
            reg_.commands().remove<Parent>(entity);
        }
        return;
    }

    auto& parent = reg_.get_mut<Parent>(ent_);

    HierarchyTool tool(reg_, entity);
    auto& childParent = tool.disconnectParentOrCreate(reg_);
    childParent.entity = parent.entity;

    auto& children = reg_.get_mut<Child>(parent.entity).entities;
    auto it = std::find(children.begin(), children.end(), ent_);
    if (it == children.end()) {
        return;
    }

    children.insert(it + 1, entity);
}

Parent& HierarchyTool::disconnectParentOrCreate(gecs::registry reg) const {
    if (!HasHierarchy() || IsRoot(reg_, ent_)) {
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
    if (IsRoot(reg_, ent_)) {
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