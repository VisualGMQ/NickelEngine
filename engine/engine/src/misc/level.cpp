#include "nickel/misc/Level.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/nickel.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel {

void Level::Update() {
    preorderGO(nullptr, m_root_go);
}

void Level::preorderGO(GameObject* parent, GameObject& go) {
    go.UpdateGlobalTransform(parent ? parent->GetGlobalTransform()
                                    : Transform{});
    Transform render_transform = go.GetGlobalTransform();

    if (go.m_rigid_actor) {
        go.m_global_transform = go.m_rigid_actor.GetGlobalTransform();
        // NOTE: hack back render scale
        go.m_global_transform.scale = render_transform.scale;
        go.m_transform =
            parent
                ? go.m_global_transform.RelatedBy(parent->GetGlobalTransform())
                : go.m_global_transform;
    }
    if (go.m_controller) {
        auto p = go.m_controller.GetFootPosition();
        go.m_global_transform.p = p;
        go.m_transform =
            parent
                ? go.m_global_transform.RelatedBy(parent->GetGlobalTransform())
                : go.m_global_transform;
    }
    if (go.m_skeleton) {
        go.m_skeleton.UpdateTransformByRoot(go.m_global_transform);
    }

    if (go.m_model && go.m_enable_render_model) {
        Context::GetInst().GetGraphicsContext().DrawModel(go.m_global_transform,
                                                          go.m_model);
    }

    for (auto& child : go.m_children) {
        preorderGO(&go, child);
    }
}

}  // namespace nickel