#include "nickel/debugger/debugger.hpp"

#include "nickel/animation/internal/skeleton_impl.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/context.hpp"
#include "nickel/physics/internal/scene_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::debugger {

void drawOnePhysicActor(const physx::PxActor* actor) {
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
                std::vector<Vec3> vertices;
                vertices.resize(mesh->getNbVertices());
                auto indices = mesh->getIndexBuffer();

                auto transform =
                    global_transform *
                    Transform{
                        Vec3{}, physics::Vec3FromPhysX(convex_mesh.scale.scale),
                        physics::QuatFromPhysX(convex_mesh.scale.rotation)};

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

                for (uint32_t i = 0; i < mesh->getNbPolygons(); i++) {
                    physx::PxHullPolygon face;
                    NICKEL_CONTINUE_IF_FALSE(mesh->getPolygonData(i, face));

                    const physx::PxU8* face_indices = indices + face.mIndexBase;

                    for (uint32_t j = 1; j < face.mNbVerts; j++) {
                        const Vec3& p2 = vertices[face_indices[j]];
                        const Vec3& p1 = vertices[face_indices[j - 1]];
                        debug_drawer.DrawLine(p1, p2, color, color);
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
                debug_drawer.DrawCapsule(
                    global_transform.p, capsule.halfHeight, capsule.radius,
                    global_transform.q *
                        Quat::Create(Vec3{0, 0, 1}, Degrees{90}),
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

void DebugManager::EnabelPhysicsDebugDraw(bool enable) {
    m_enable_physics_debug_draw = enable;
}

bool DebugManager::IsPhysicsDebugDrawEnabled() const {
    return m_enable_physics_debug_draw;
}

void DebugManager::EnabelSkeletonDebugDraw(bool enable) {
    m_enable_skeleton_debug_draw = enable;
}

bool DebugManager::IsSkeletonDebugDrawEnabled() const {
    return m_enable_skeleton_debug_draw;
}

void DebugManager::Update() {
    updateImGui();
    auto& level = Context::GetInst().GetCurrentLevel();

    if (IsPhysicsDebugDrawEnabled()) {
        drawPhysicActors();
    }
    
    auto& root_go = level.GetRootGO();
    if (IsSkeletonDebugDrawEnabled()) {
        drawGOSkeletonRecurse(root_go);
    }
    
    enableModelRenderRecurse(root_go, m_enable_model_render);
}

void DebugManager::drawPhysicActors() {
    auto& physics_ctx = Context::GetInst().GetPhysicsContext();
    auto scene = physics_ctx.GetMainScene().GetImpl()->m_scene;
    std::vector<physx::PxActor*> actors;

    auto required_actor_type = physx::PxActorTypeFlag::eRIGID_STATIC |
                               physx::PxActorTypeFlag::eRIGID_DYNAMIC;
    actors.resize(scene->getNbActors(required_actor_type));
    scene->getActors(required_actor_type, actors.data(), actors.size());

    for (auto actor : actors) {
        drawOnePhysicActor(actor);
    }
}

void DebugManager::drawSkeleton(const Skeleton& skeleton) {
    auto impl = skeleton.GetImpl();
    drawSkeletonRecurse(skeleton, impl->m_root_bone_idx);
}

void DebugManager::drawSkeletonRecurse(const Skeleton& skeleton,
                                       uint32_t bone_idx) {
    auto impl = skeleton.GetImpl();
    auto& bone = impl->m_bones[bone_idx];

    auto& debug_drawer = Context::GetInst().GetDebugDrawer();
    debug_drawer.DrawSphere(bone.m_global_transform.p, 0.01,
                            bone.m_global_transform.q, Color{1, 0, 0, 1}, true);

    auto& children = impl->m_hierarchy[bone_idx];
    for (auto child : children) {
        auto& child_bone = impl->m_bones[child];
        debug_drawer.DrawLine(bone.m_global_transform.p,
                              child_bone.m_global_transform.p,
                              Color{1, 0, 0, 1}, Color{1, 0, 0, 1});
        drawSkeletonRecurse(skeleton, child);
    }
}

void DebugManager::drawGOSkeletonRecurse(const GameObject& go) {
    if (go.m_skeleton) {
        drawSkeleton(go.m_skeleton);
    }

    for (auto& child : go.m_children) {
        drawGOSkeletonRecurse(child);
    }
}

void DebugManager::enableModelRenderRecurse(GameObject& go, bool enable) {
    go.m_enable_render_model = enable;

    for (auto& child : go.m_children) {
        enableModelRenderRecurse(child, enable);
    }
}

void DebugManager::updateImGui() {
    if (ImGui::Begin("debug window", &m_imgui_window_open)) {
        ImGui::Checkbox("show demo window", &m_show_demo_window);
        {
            bool enable = IsPhysicsDebugDrawEnabled();
            ImGui::Checkbox("draw physics actors", &enable);
            EnabelPhysicsDebugDraw(enable);
        }
        {
            bool enable = IsSkeletonDebugDrawEnabled();
            ImGui::Checkbox("draw skeletons", &enable);
            EnabelSkeletonDebugDraw(enable);
        }
        ImGui::Checkbox("draw models", &m_enable_model_render);
    }
    ImGui::End();

    ImGui::ShowDemoWindow(&m_show_demo_window);
}

}  // namespace nickel::debugger
