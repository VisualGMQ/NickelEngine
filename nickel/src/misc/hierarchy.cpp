#include "misc/hierarchy.hpp"

namespace nickel {

void doUpdateGlobalTransform(gecs::entity entity, Transform& trans,
                             GlobalTransform& globalTrans, gecs::registry reg) {
    if (!reg.has<Child>(entity)) return;

    auto& child = reg.get<Child>(entity);

    for (auto ent : child.entities) {
        if (!reg.has<Transform>(ent) && reg.has<GlobalTransform>(ent)) continue;
        auto& childTrans = reg.get_mut<Transform>(ent);
        auto& childGlobalTrans = reg.get_mut<GlobalTransform>(ent);
        childGlobalTrans.mat = globalTrans.mat * childTrans.ToMat();
        doUpdateGlobalTransform(ent, childTrans, childGlobalTrans, reg);
    }
}

void UpdateGlobalTransform(
    gecs::querier<gecs::mut<GlobalTransform>, gecs::mut<Transform>, Parent>
        querier,
    gecs::registry reg) {
    for (auto&& [entity, gTrans, trans, parent] : querier) {
        if (parent.entity == gecs::null_entity) {
            doUpdateGlobalTransform(entity, trans, gTrans, reg);
        }
    }
}

}  // namespace nickel