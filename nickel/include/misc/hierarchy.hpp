#pragma once

#include "pch.hpp"
#include "gecs/entity/entity.hpp"
#include "misc/transform.hpp"

namespace nickel {

/**
 * @brief [component] save parent entity
 */
struct Parent {
    gecs::entity entity;
};

/**
 * @brief [component] save child entities
 */
struct Child {
    std::vector<gecs::entity> entities;
};

void UpdateGlobalTransform(
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>, Child,
                  gecs::without<Parent>>,
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>,
                  gecs::without<Parent>>,
    gecs::registry);

struct HierarchyTool final {
public:
    explicit HierarchyTool(gecs::entity ent): ent_(ent) {}

    auto GetEntity() const { return ent_; }

    /**
     * @brief make entity as child
     */
    void MoveEntityAsChild(gecs::entity other,
                           std::optional<size_t> idx = std::nullopt);

    /**
     * @brief change child entity order
     * @note require childEnt is child
     */
    void ChangeOrder(gecs::entity childEnt, size_t idx) const;

    void MoveAsSibling(gecs::entity entity) const;

    static bool HasHierarchy(gecs::entity ent) {
        gecs::registry reg(*gWorld->cur_registry());
        return reg.alive(ent) && (reg.has<Child>(ent) || reg.has<Parent>(ent));
    }

    static bool IsRoot(gecs::entity ent) {
        gecs::registry reg(*gWorld->cur_registry());
        return reg.alive(ent) && reg.has<Child>(ent) && !reg.has<Parent>(ent);
    }

    static bool IsLeaf(gecs::entity ent) {
        gecs::registry reg(*gWorld->cur_registry());
        return reg.alive(ent) && !reg.has<Child>(ent) && reg.has<Parent>(ent);
    }

    static bool IsNode(gecs::entity ent) {
        gecs::registry reg(*gWorld->cur_registry());
        return reg.alive(ent) && reg.has<Child>(ent) && reg.has<Parent>(ent);
    }

    bool IsRoot() const { return IsRoot(ent_); }

    bool IsLeaf() const { return IsLeaf(ent_); }

    bool IsNode() const { return IsNode(ent_); }

    bool HasHierarchy() const { return HasHierarchy(ent_); }

private:
    gecs::entity ent_;

    Parent& disconnectParentOrCreate(gecs::registry) const;
    Parent* disconnectParent(gecs::registry reg) const;
};

}  // namespace nickel