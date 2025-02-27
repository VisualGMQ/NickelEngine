#include "nickel/misc/Level.hpp"
#include "nickel/nickel.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel {

void Level::Update() {
    preorderGO(nullptr, m_root_go);
}

void Level::preorderGO(GameObject* parent, GameObject& go) {
    go.UpdateGlobalTransform(parent ? parent->GetGlobalTransform()
                                    : Transform{});
    Transform global_transform = go.GetGlobalTransform();

    if (go.m_rigid_actor) {
        go.m_global_transform = go.m_rigid_actor.GetGlobalTransform();
        go.m_transform =
            parent
                ? go.m_global_transform.RelatedBy(parent->GetGlobalTransform())
                : go.m_global_transform;
        debugDrawRigidActor(go);
    }

    if (go.m_model) {
        Context::GetInst().GetGraphicsContext().DrawModel(global_transform,
                                                          go.m_model);
    }

    for (auto& child : go.m_children) {
        preorderGO(&go, child);
    }
}

void Level::debugDrawRigidActor(const GameObject& go) {
    auto shapes = go.m_rigid_actor.GetShapes();
    auto& debug_drawer = Context::GetInst().GetDebugDrawer();
    for (auto& shape : shapes) {
        auto& geom = shape.GetImpl()->m_shape->getGeometry();
        auto local_transform = physics::TransformFromPhysX(
            shape.GetImpl()->m_shape->getLocalPose());
        Transform global_transform = go.GetGlobalTransform() * local_transform;
        physx::PxGeometryHolder holder{geom};

        // TODO: draw all physics mesh
        switch (geom.getType()) {
            case physx::PxGeometryType::eBOX: {
                auto& box = holder.box();
                debug_drawer.DrawBox(global_transform.p,
                                     physics::Vec3FromPhysX(box.halfExtents),
                                     global_transform.q, Color{0, 1, 0, 1});
            } break;
            case physx::PxGeometryType::eSPHERE:
            case physx::PxGeometryType::ePLANE:
            case physx::PxGeometryType::eCAPSULE:
            case physx::PxGeometryType::eCONVEXCORE:
            case physx::PxGeometryType::eCONVEXMESH:
            case physx::PxGeometryType::ePARTICLESYSTEM:
            case physx::PxGeometryType::eTETRAHEDRONMESH:
            case physx::PxGeometryType::eTRIANGLEMESH:
            case physx::PxGeometryType::eHEIGHTFIELD:
            case physx::PxGeometryType::eCUSTOM:
            case physx::PxGeometryType::eGEOMETRY_COUNT:
            case physx::PxGeometryType::eINVALID:
                NICKEL_CANT_REACH();
                break;
        }
    }
}

}  // namespace nickel