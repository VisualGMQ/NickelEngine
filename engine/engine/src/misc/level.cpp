﻿#include "nickel/misc/Level.hpp"
#include "nickel/common/macro.hpp"
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
                                              physics::Vec3FromPhysX(p2),
                                              {0, 1, 0, 1}, {0, 1, 0, 1});
                    }
                }
            } break;
            case physx::PxGeometryType::eSPHERE:
            case physx::PxGeometryType::ePLANE:
            case physx::PxGeometryType::eCAPSULE:
            case physx::PxGeometryType::eCONVEXCORE:
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