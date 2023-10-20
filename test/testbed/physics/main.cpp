#include "core/cgmath.hpp"
#include "gecs/entity/fwd.hpp"
#include "gecs/entity/querier.hpp"
#include "gecs/entity/resource.hpp"
#include "geom/geom2d.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "physics/body.hpp"
#include "physics/circle_shape.hpp"
#include "physics/obb_shape.hpp"
#include "physics/polygon_shape.hpp"
#include "physics/shape.hpp"
#include "physics/world.hpp"

#include "input/device.hpp"
#include "misc/project.hpp"
#include "nickel.hpp"
#include "renderer/renderer2d.hpp"

#include <type_traits>
#include <unordered_map>
#include <variant>

using namespace nickel;

#define WindowWidth 1024
#define WindowHeight 720

constexpr float WindowCenterX = WindowWidth / 2.0f;
constexpr float WindowCenterY = WindowHeight / 2.0f;

void ImGuiInit(gecs::resource<gecs::mut<Window>> window,
               gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // enable Docking

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window->Raw(), true);
    ImGui_ImplOpenGL3_Init("#version 430");

    renderer2d->SetClearColor({0.1f, 0.1f, 0.1f, 1.0});
}

void ImGuiStart() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EditorShutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiEnd(gecs::resource<gecs::mut<Window>> window,
              gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize((GLFWwindow*)window->Raw(), &display_w, &display_h);
    renderer2d->SetViewport({0, 0}, cgmath::Vec2(display_w, display_h));
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TestbedStartup(gecs::commands cmds, gecs::resource<gecs::mut<physics::World>> world) {
    auto ent1 = cmds.create();
    cmds.emplace<physics::Body>(
        ent1, physics::Body::CreateDynamic(cgmath::Vec2{200, 200}));
    auto& shape = cmds.emplace<physics::CollideShape>(
        ent1, physics::CircleShape::FromCenter(cgmath::Vec2{}, 30));

    world->forceGenerators.push_back([](physics::Body& b) {
        b.force = cgmath::Vec2{0, 9.8};
    });
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
                auto& s =
                    physics::shape_cast<const physics::OBBShape&>(*shape.shape)
                        .shape;
                renderer->DrawLineLoop(
                    std::vector<cgmath::Vec2>{
                        s.center - s.axis * s.halfLen,
                        cgmath::Vec2{s.center.x + s.axis.x * s.halfLen.w,
                                     s.center.y - s.axis.y * s.halfLen.h},
                        s.center + s.axis * s.halfLen,
                        {s.center.x - s.axis.x * s.halfLen.w,
                                     s.center.y + s.axis.y * s.halfLen.h}
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

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo initInfo;
    initInfo.windowData.title = "geometric playground";
    initInfo.windowData.size.Set(WindowWidth, WindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.commands().emplace_resource<ProjectInitInfo>(std::move(initInfo));

    reg.regist_startup_system<ImGuiInit>()
        .regist_startup_system<physics::PhysicsInit>()
        .regist_startup_system<TestbedStartup>()
        .regist_update_system<RenderBodies>()
        .regist_update_system<RenderShapes>()
        .regist_update_system<ImGuiStart>()
        .regist_update_system<ImGuiEnd>()
        .regist_update_system<physics::PhysicsUpdate>();
}
