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

}  // namespace nickel