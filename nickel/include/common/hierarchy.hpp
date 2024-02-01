#pragma once

#include "common/transform.hpp"
#include "gecs/gecs.hpp"

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
    explicit HierarchyTool(gecs::registry reg, gecs::entity ent) : reg_{reg}, ent_(ent) {}

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
    void ChangeOrder(gecs::entity childEnt, size_t idx);

    void MoveAsSibling(gecs::entity entity);

    static bool HasHierarchy(gecs::registry reg, gecs::entity ent) {
        return reg.alive(ent) && (reg.has<Child>(ent) || reg.has<Parent>(ent));
    }

    static bool IsRoot(gecs::registry reg, gecs::entity ent) {
        return reg.alive(ent) && reg.has<Child>(ent) && !reg.has<Parent>(ent);
    }

    static bool IsLeaf(gecs::registry reg, gecs::entity ent) {
        return reg.alive(ent) && !reg.has<Child>(ent) && reg.has<Parent>(ent);
    }

    static bool IsNode(gecs::registry reg, gecs::entity ent) {
        return reg.alive(ent) && reg.has<Child>(ent) && reg.has<Parent>(ent);
    }

    bool IsRoot() const { return IsRoot(reg_, ent_); }

    bool IsLeaf() const { return IsLeaf(reg_, ent_); }

    bool IsNode() const { return IsNode(reg_, ent_); }

    bool HasHierarchy() const { return HasHierarchy(reg_, ent_); }

    using PreorderVisitFn = std::function<void(gecs::entity, gecs::registry)>;

    void PreorderVisit(const PreorderVisitFn& fn);

private:
    gecs::registry reg_;
    gecs::entity ent_;

    Parent& disconnectParentOrCreate(gecs::registry) const;
    Parent* disconnectParent(gecs::registry reg) const;

    void doPreorderVisit(gecs::entity ent, const PreorderVisitFn& fn,
                         gecs::registry reg) {
        fn(ent, reg);
        if (reg.has<Child>(ent)) {
            auto& children = reg.get<Child>(ent).entities;
            for (auto child : children) {
                doPreorderVisit(child, fn, reg);
            }
        }
    }
};

}  // namespace nickel