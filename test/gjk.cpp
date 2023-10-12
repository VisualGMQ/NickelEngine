#include "misc/project.hpp"
#include "nickel.hpp"
#include "refl/anim.hpp"

using namespace nickel;

struct Geometry {
    std::vector<cgmath::Vec2> pts;
};

struct CanMove {};

void InitGeoms(gecs::commands cmds) {
    auto entity1 = cmds.create();
    cmds.emplace<Geometry>(entity1,
                           Geometry{
                               {{-20, 70}, {-60, 30}, {-30, -30}, {40, -40}}
    });
    cmds.emplace<Transform>(entity1,
                            Transform::FromTranslation(cgmath::Vec2{237, 290}));

    auto entity2 = cmds.create();
    cmds.emplace<Geometry>(entity2,
                           Geometry{
                               {{-20, 70}, {-60, 30}, {-30, -30}, {40, -40}}
    });
    cmds.emplace<CanMove>(entity2);
    cmds.emplace<Transform>(entity2,
                            Transform::FromTranslation(cgmath::Vec2{176, 376}));
}

void RenderGeoms(gecs::querier<Transform, Geometry> querier,
                 gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto&& [_, trans, geom] : querier) {
        for (int i = 0; i <= geom.pts.size(); i++) {
            renderer->DrawLine(
                geom.pts[i % geom.pts.size()] * trans.scale + trans.translation,
                geom.pts[(i + 1) % geom.pts.size()] * trans.scale +
                    trans.translation,
                cgmath::Color{1, 1, 0, 1});
        }
    }
}

void DoGjk(gecs::querier<Geometry, Transform> querier,
           gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto it = querier.begin(); it != querier.end() - 1; it++) {
        for (auto it2 = querier.begin() + 1; it2 != querier.end(); it2++) {
            auto geom1 = std::get<1>(*it);
            auto& trans1 = std::get<2>(*it);
            auto geom2 = std::get<1>(*it2);
            auto& trans2 = std::get<2>(*it2);

            for (auto& pt : geom1.pts) {
                pt = pt * trans1.scale + trans1.translation;
            }

            for (auto& pt : geom2.pts) {
                pt = pt * trans2.scale + trans2.translation;
            }

            if (geom2d::Gjk(geom1.pts, geom2.pts)) {
                LOGI("intersected", "intersected");
            }
        }
    }
}

void MoveGeom(gecs::querier<gecs::mut<Transform>, CanMove> querier,
              gecs::resource<Mouse> mouse) {
    for (auto&& [_, trans, m] : querier) {
        trans.translation = mouse->Position();
    }
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo info;
    info.windowData.title = "gjk-test";
    info.windowData.size.Set(1024, 720);
    info.projectPath = "./sandbox";

    InitSystem(world, info, reg.commands());

    reg.regist_update_system<RenderGeoms>()
        .regist_update_system<MoveGeom>()
        .regist_update_system<DoGjk>()
        .regist_startup_system<InitGeoms>();
}
