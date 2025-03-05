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
            case physx::PxGeometryType::eTRIANGLEMESH: {
                auto& triangle_mesh = holder.triangleMesh();
                auto mesh = triangle_mesh.triangleMesh;
                std::vector<Vec3> vertices;
                vertices.resize(mesh->getNbVertices());
                for (int i = 0; i < mesh->getNbVertices(); i++) {
                    Vec3 p = physics::Vec3FromPhysX(mesh->getVertices()[i]);
                    p = (global_transform *
                         Transform{
                             p,
                             physics::Vec3FromPhysX(triangle_mesh.scale.scale),
                             physics::QuatFromPhysX(
                                 triangle_mesh.scale.rotation)})
                            .p;
                    vertices[i] = p;
                }
                if (mesh->getTriangleMeshFlags() |
                    physx::PxTriangleMeshFlag::e16_BIT_INDICES) {
                    debug_drawer.DrawTriangleMesh(
                        std::span(vertices),
                        std::span((uint16_t*)mesh->getTriangles(),
                                  mesh->getNbTriangles() * 3),
                        Color{0, 1, 0, 1});
                } else {
                    debug_drawer.DrawTriangleMesh(
                        std::span(vertices),
                        std::span((uint32_t*)mesh->getTriangles(),
                                  mesh->getNbTriangles() * 3),
                        Color{0, 1, 0, 1});
                }
                break;
            }
            case physx::PxGeometryType::eSPHERE:
            case physx::PxGeometryType::ePLANE:
            case physx::PxGeometryType::eCAPSULE:
            case physx::PxGeometryType::eCONVEXCORE:
            case physx::PxGeometryType::eCONVEXMESH:
            case physx::PxGeometryType::ePARTICLESYSTEM:
            case physx::PxGeometryType::eTETRAHEDRONMESH:
            case physx::PxGeometryType::eHEIGHTFIELD:
            default:
                NICKEL_CANT_REACH();
                break;
        }
    }
}

}  // namespace nickel