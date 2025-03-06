#include "nickel/misc/Level.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/nickel.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/shape_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel {
void debugDrawRigidActor(const physx::PxActor* actor) {
    Color color = Color{1, 1, 1, 1};
    const physx::PxRigidActor* rigid_actor = actor->is<physx::PxRigidActor>();
    NICKEL_RETURN_IF_FALSE(rigid_actor);
    switch (rigid_actor->getType()) {
        case physx::PxActorType::eRIGID_STATIC:
            color = Color{0, 1, 0, 1};
            break;
        case physx::PxActorType::eRIGID_DYNAMIC:
            color = Color{1, 0, 0, 1};
            break;
        case physx::PxActorType::eARTICULATION_LINK:
            color = Color{1, 1, 0, 1};
            break;
        case physx::PxActorType::eDEFORMABLE_SURFACE:
            color = Color{0, 1, 1, 1};
            break;
        case physx::PxActorType::eDEFORMABLE_VOLUME:
            color = Color{0, 1, 1, 1};
            break;
        case physx::PxActorType::ePBD_PARTICLESYSTEM:
            color = Color{1, 0, 1, 1};
            break;
        default:
            NICKEL_CANT_REACH();
    }
    auto& debug_drawer = Context::GetInst().GetDebugDrawer();

    std::vector<physx::PxShape*> shapes;
    shapes.resize(rigid_actor->getNbShapes());
    rigid_actor->getShapes(shapes.data(), shapes.size());

    for (auto& shape : shapes) {
        auto& geom = shape->getGeometry();
        auto local_transform = shape->getLocalPose();
        auto global_transform = physics::TransformFromPhysX(
            rigid_actor->getGlobalPose() * local_transform);
        physx::PxGeometryHolder holder{geom};

        // TODO: draw all physics mesh
        switch (geom.getType()) {
            case physx::PxGeometryType::eBOX: {
                auto& box = holder.box();
                debug_drawer.DrawBox(global_transform.p,
                                     physics::Vec3FromPhysX(box.halfExtents),
                                     global_transform.q, color);
            } break;
            case physx::PxGeometryType::eTRIANGLEMESH: {
                auto& triangle_mesh = holder.triangleMesh();
                auto mesh = triangle_mesh.triangleMesh;

                auto transform =
                    global_transform *
                    Transform{
                        Vec3{},
                        physics::Vec3FromPhysX(triangle_mesh.scale.scale),
                        physics::QuatFromPhysX(triangle_mesh.scale.rotation)};

                std::vector<Vec3> vertices;
                vertices.resize(mesh->getNbVertices());
                std::ranges::transform(
                    std::span{mesh->getVertices(), mesh->getNbVertices()},
                    vertices.begin(), [&](const physx::PxVec3& v) {
                        return (transform *
                                Transform{
                                    physics::Vec3FromPhysX(v), {1, 1, 1},
                                     {}
                        })
                            .p;
                    });

                if (mesh->getTriangleMeshFlags() |
                    physx::PxTriangleMeshFlag::e16_BIT_INDICES) {
                    debug_drawer.DrawTriangleMesh(
                        std::span(vertices),
                        std::span((uint16_t*)mesh->getTriangles(),
                                  mesh->getNbTriangles() * 3),
                        color);
                } else {
                    debug_drawer.DrawTriangleMesh(
                        std::span(vertices),
                        std::span((uint32_t*)mesh->getTriangles(),
                                  mesh->getNbTriangles() * 3),
                        color);
                }
                break;
            }
            case physx::PxGeometryType::eCONVEXMESH: {
                auto& convex_mesh = holder.convexMesh();
                auto mesh = convex_mesh.convexMesh;
                auto vertices =
                    std::span{mesh->getVertices(), mesh->getNbVertices()};
                auto indices = mesh->getIndexBuffer();

                for (uint32_t i = 0; i < mesh->getNbPolygons(); i++) {
                    physx::PxHullPolygon face;
                    NICKEL_CONTINUE_IF_FALSE(mesh->getPolygonData(i, face));

                    const physx::PxU8* face_indices = indices + face.mIndexBase;

                    for (uint32_t j = 1; j < face.mNbVerts; j++) {
                        const physx::PxVec3& p2 = vertices[face_indices[j]];
                        const physx::PxVec3& p1 = vertices[face_indices[j - 1]];
                        debug_drawer.DrawLine(physics::Vec3FromPhysX(p1),
                                              physics::Vec3FromPhysX(p2), color,
                                              color);
                    }
                }
            } break;
            case physx::PxGeometryType::eSPHERE: {
                auto& sphere = holder.sphere();
                debug_drawer.DrawSphere(global_transform.p, sphere.radius,
                                        global_transform.q, color, true);
            } break;
            case physx::PxGeometryType::eCAPSULE: {
                auto& capsule = holder.capsule();
                debug_drawer.DrawCapsule(global_transform.p, capsule.halfHeight,
                                         capsule.radius, global_transform.q,
                                         color, true);
            } break;
            case physx::PxGeometryType::ePLANE:
            case physx::PxGeometryType::eCONVEXCORE:
            case physx::PxGeometryType::ePARTICLESYSTEM:
            case physx::PxGeometryType::eTETRAHEDRONMESH:
            case physx::PxGeometryType::eHEIGHTFIELD:
            default:
                // NICKEL_CANT_REACH();
                break;
        }
    }
}

void Level::Update() {
    preorderGO(nullptr, m_root_go);

    auto& physics_ctx = Context::GetInst().GetPhysicsContext();
    auto scene = physics_ctx.GetMainScene().GetImpl()->m_scene;
    std::vector<physx::PxActor*> actors;

    auto required_actor_type = physx::PxActorTypeFlag::eRIGID_STATIC |
                               physx::PxActorTypeFlag::eRIGID_DYNAMIC;
    actors.resize(scene->getNbActors(required_actor_type));
    scene->getActors(required_actor_type, actors.data(), actors.size());

    for (auto actor : actors) {
        debugDrawRigidActor(actor);
    }
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

    if (go.m_model) {
        Context::GetInst().GetGraphicsContext().DrawModel(go.m_global_transform,
                                                          go.m_model);
    }

    for (auto& child : go.m_children) {
        preorderGO(&go, child);
    }
}

}  // namespace nickel