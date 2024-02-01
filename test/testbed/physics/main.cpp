#include "nickel.hpp"

#include "imgui_plugin.hpp"

#include "physics/body.hpp"
#include "physics/circle_shape.hpp"
#include "physics/obb_shape.hpp"
#include "physics/polygon_shape.hpp"
#include "physics/shape.hpp"
#include "physics/world.hpp"

#include "video/device.hpp"
#include "misc/project.hpp"
#include "graphics/renderer2d.hpp"

#include <type_traits>
#include <unordered_map>
#include <variant>

using namespace nickel;

#define WindowWidth 1024
#define WindowHeight 720

constexpr float WindowCenterX = WindowWidth / 2.0f;
constexpr float WindowCenterY = WindowHeight / 2.0f;

struct Control {};

void TestbedStartup(gecs::commands cmds,
                    gecs::resource<gecs::mut<physics::World>> world) {
    world->forceGenerators.emplace_back([](physics::Body& b) {
        b.force += cgmath::Vec2{0, 0.98};
    });

    auto ent2 = cmds.create();
    cmds.emplace<physics::Body>(ent2, physics::Body::CreateStatic({500, 500}));
    cmds.emplace<physics::CollideShape>(
        ent2, physics::OBBShape::FromCenter({}, {400, 25}, 0.0));

    // auto ent3 = cmds.create();
    // cmds.emplace<physics::Body>(
    //     ent3, physics::Body::CreateStatic({500, 500}));
    // cmds.emplace<physics::CollideShape>(
    //     ent3, physics::CircleShape::FromCenter(cgmath::Vec2{}, 30));
    // cmds.emplace<Control>(ent3);
}

void RenderBodies(gecs::querier<physics::Body> bodies,
                  gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto&& [_, body] : bodies) {
        renderer->DrawCircle(body.pos, 5, {0, 1, 0, 1});
    }
}

void RenderShapes(gecs::querier<physics::Body, physics::CollideShape> querier,
                  gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto&& [_, body, shape] : querier) {
        switch (shape.shape->GetType()) {
            case physics::Shape::Type::Circle: {
                auto& c = physics::shape_cast<const physics::CircleShape&>(
                    *shape.shape);
                renderer->DrawCircle(c.shape.center + body.pos, c.shape.radius,
                                     {1, 0, 1, 1});
            } break;
            case physics::Shape::Type::OBB: {
                // TODO: currently can only render AABB(OBB with 0 rotation)
                auto& s =
                    physics::shape_cast<const physics::OBBShape&>(*shape.shape)
                        .shape;
                auto&& [xAxis, yAxis] = s.GetAxis();
                renderer->DrawLineLoop(
                    std::vector<cgmath::Vec2>{
                        body.pos + s.center + (-xAxis - yAxis) * s.halfLen,
                        body.pos + s.center + (xAxis - yAxis) * s.halfLen,
                        body.pos + s.center + (xAxis + yAxis) * s.halfLen,
                        body.pos + s.center + (-xAxis + yAxis) * s.halfLen,
                    },
                    {1, 0, 1, 1});
            } break;
            case physics::Shape::Type::Polygon: {
                auto& pts = physics::shape_cast<const physics::PolygonShape&>(
                                *shape.shape)
                                .shape;
                renderer->DrawLineLoop(pts, {1, 0, 1, 1});

            } break;
            case physics::Shape::Type::Capsule: {
                auto& c = physics::shape_cast<const physics::CapsuleShape&>(
                              *shape.shape)
                              .shape;
                renderer->DrawLine(c.seg.p, c.seg.p + c.seg.dir * c.seg.len,
                                   {1, 0, 1, 1});
                renderer->DrawCircle(c.seg.p, c.radius, {1, 0, 1, 1});
                renderer->DrawCircle(c.seg.p + c.seg.dir * c.seg.len, c.radius,
                                     {1, 0, 1, 1});
            } break;
        }
    }
}

void ShootCircle(gecs::commands cmds, gecs::resource<Mouse> mouse,
                 gecs::querier<gecs::mut<physics::Body>, Control> querier) {
    if (mouse->LeftBtn().IsPressed()) {
        auto ent = cmds.create();
        auto& body = cmds.emplace<physics::Body>(
            ent, physics::Body::CreateDynamic(cgmath::Vec2{400, 200}));
        body.force = cgmath::Normalize(mouse->Position() - body.pos) * 10;
        auto& shape = cmds.emplace<physics::CollideShape>(
            ent, physics::CircleShape::FromCenter(cgmath::Vec2{}, 30));
    }

    if (mouse->RightBtn().IsPressed()) {
        auto ent1 = cmds.create();
        cmds.emplace<physics::Body>(
            ent1, physics::Body::CreateDynamic(cgmath::Vec2{200, 200}));
        auto& shape = cmds.emplace<physics::CollideShape>(
            ent1, physics::CircleShape::FromCenter(cgmath::Vec2{}, 30));
    }

    for (auto&& [_, body, control] : querier) {
        body.pos = mouse->Position();
    }
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo initInfo;
    initInfo.windowData.title = "geometric playground";
    initInfo.windowData.size.Set(WindowWidth, WindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.commands().emplace_resource<ProjectInitInfo>(std::move(initInfo));

    reg.regist_startup_system<plugin::ImGuiInit>()
        .regist_startup_system<physics::PhysicsInit>()
        .regist_startup_system<TestbedStartup>()
        .regist_update_system<ShootCircle>()
        .regist_update_system<physics::PhysicsUpdate>()
        .regist_update_system<RenderBodies>()
        .regist_update_system<RenderShapes>()
        .regist_update_system<plugin::ImGuiStart>()
        .regist_update_system<plugin::ImGuiEnd>()
        .regist_shutdown_system<plugin::ImGuiShutdown>();
}
