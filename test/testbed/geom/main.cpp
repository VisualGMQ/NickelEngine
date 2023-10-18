#include "core/cgmath.hpp"
#include "gecs/entity/querier.hpp"
#include "gecs/entity/resource.hpp"
#include "geom/basic_geom.hpp"
#include "geom/geom2d.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

const cgmath::Color GeomColor = {0.0, 1.0, 0.0, 1.0};

template <typename T>
using Polygon = std::vector<cgmath::Vec<T, 2>>;

enum class PolygonIntersectMode {
    GJK = 0,
    GJKNearest = 1,
    SAT = 2,
};

struct Context final {
    struct IntersectInfo final {
        cgmath::Vec2 p1, p2;

        cgmath::Vec2 center;
        geom2d::MTV<float, 2> mtv;
    };

    std::vector<std::variant<geom2d::Line<float>, geom2d::Ray<float>,
                             geom2d::AABB<float>, geom2d::Circle<float>,
                             geom2d::Segment<float>, geom2d::Capsule<float>,
                             Polygon<float>>>
        geoms;
    bool showNearestPoint = false;
    bool showIntersect = false;
    PolygonIntersectMode intersectMode = PolygonIntersectMode::SAT;
    bool useEPA = false;
    cgmath::Color intersectColor = cgmath::Color{1, 0, 0, 1};
    cgmath::Color nearestPtColor = cgmath::Color{1, 1, 0, 1};
    std::vector<bool> intersected;
    std::vector<IntersectInfo> intersectResult;
};

void PlaygroundStartup(gecs::commands cmds) {
    cmds.emplace_resource<Context>();
}

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

using GeometricAddFunc = void (*)(Context&);

void AddAABB(Context& ctx) {
    ctx.geoms.push_back(geom2d::AABB<float>::FromCenter(
        cgmath::Vec2{WindowCenterX, WindowCenterY}, cgmath::Vec2{50, 50}));
}

void AddCircle(Context& ctx) {
    ctx.geoms.push_back(geom2d::Circle<float>::Create(
        cgmath::Vec2{WindowCenterX, WindowCenterY}, 50));
}

void AddLine(Context& ctx) {
    ctx.geoms.push_back(geom2d::Line<float>::FromDir(
        cgmath::Vec2{WindowCenterX, WindowCenterY}, cgmath::Vec2{1, 0}));
}

void AddSegment(Context& ctx) {
    ctx.geoms.push_back(geom2d::Segment<float>::FromPts(
        cgmath::Vec2{WindowCenterX, WindowCenterY},
        cgmath::Vec2{WindowCenterX + 100, 0}));
}

void AddCapsule(Context& ctx) {
    ctx.geoms.push_back(geom2d::Capsule<float>::Create(
        geom2d::Segment<float>::FromPts(
            cgmath::Vec2{WindowCenterX - 50, WindowCenterY},
            cgmath::Vec2{WindowCenterX + 50, WindowCenterY}),
        50));
}

void AddPolygon(Context& ctx) {
    ctx.geoms.push_back(std::vector<cgmath::Vec2>{
        {WindowCenterX - 50, WindowCenterY - 50},
        {WindowCenterX + 50, WindowCenterY - 50},
        {WindowCenterX + 50, WindowCenterY + 50},
        {WindowCenterX - 50, WindowCenterY + 50}
    });
}

std::unordered_map<std::string_view, GeometricAddFunc> GemetricAddMethods = {
    {   "add AABB",    AddAABB},
    { "add Circle",  AddCircle},
    {   "add Line",    AddLine},
    {"add Segment", AddSegment},
    {"add Capsule", AddCapsule},
    {"add Polygon", AddPolygon},
};

struct GeometricRender final {
    GeometricRender(Renderer2D& renderer, const cgmath::Color& color)
        : renderer_(renderer), color_(color) {}

    void operator()(const geom2d::AABB<float>& aabb) const {
        renderer_.DrawRect(
            cgmath::Rect{aabb.center - aabb.halfLen, aabb.halfLen * 2.0},
            color_);
    }

    void operator()(const geom2d::Line<float>& line) const {
        renderer_.DrawLine(cgmath::Vec2{line.p - line.dir * 5000},
                           cgmath::Vec2{line.p + line.dir * 5000}, color_);
    }

    void operator()(const geom2d::Capsule<float>& cap) const {
        renderer_.DrawCircle(cap.seg.p, cap.radius, color_);
        renderer_.DrawCircle(cap.seg.p + cap.seg.dir * cap.seg.len, cap.radius,
                             color_);
        auto perpLine = geom2d::PerpendicularLine(cap.seg);
        renderer_.DrawLine(
            perpLine.p + perpLine.dir * cap.radius,
            perpLine.p + perpLine.dir * cap.radius + cap.seg.dir * cap.seg.len,
            color_);
        renderer_.DrawLine(
            perpLine.p - perpLine.dir * cap.radius,
            perpLine.p - perpLine.dir * cap.radius + cap.seg.dir * cap.seg.len,
            color_);
    }

    void operator()(const geom2d::Segment<float>& seg) const {
        renderer_.DrawLine(seg.p, seg.p + seg.dir * seg.len, color_);
    }

    void operator()(const geom2d::Circle<float>& c) const {
        static std::array<cgmath::Vec2, 50> pts;
        renderer_.DrawCircle(c.center, c.radius, color_);
    }

    void operator()(const geom2d::Ray<float>& r) const {
        renderer_.DrawLine(r.p, cgmath::Vec2{r.p + r.dir * 5000}, color_);
    }

    void operator()(const Polygon<float>& poly) const {
        for (int i = 0; i < poly.size(); i++) {
            renderer_.DrawLine(poly[i], poly[(i + 1) % poly.size()], color_);
        }
    }

private:
    Renderer2D& renderer_;
    cgmath::Color color_;
};

struct GeometricInfoShower final {
    explicit GeometricInfoShower(int id) : id_(id) {}

    void operator()(geom2d::AABB<float>& aabb) {
        pushIDs(3);
        if (ImGui::CollapsingHeader("AABB")) {
            ImGui::DragFloat2("center", aabb.center.data);
            ImGui::DragFloat2("half length", aabb.halfLen.data);
        }
        popIDs(3);
    }

    void operator()(geom2d::Line<float>& line) {
        pushIDs(3);
        if (ImGui::CollapsingHeader("Line")) {
            ImGui::DragFloat2("point", line.p.data);
            ImGui::DragFloat2("direction", line.dir.data);

            line.dir.Normalize();
        }
        popIDs(3);
    }

    void operator()(geom2d::Capsule<float>& cap) {
        pushIDs(4);
        if (ImGui::CollapsingHeader("Capsule")) {
            ImGui::DragFloat("radius", &cap.radius);
            ImGui::DragFloat2("p1", cap.seg.p.data);
            auto p2 = cap.seg.p + cap.seg.dir * cap.seg.len;
            ImGui::DragFloat2("p2", p2.data);

            cap.seg.dir = p2 - cap.seg.p;
            cap.seg.len = cap.seg.dir.Length();
            cap.seg.dir.Normalize();
        }
        popIDs(4);
    }

    void operator()(geom2d::Segment<float>& seg) {
        pushIDs(3);
        if (ImGui::CollapsingHeader("Segment")) {
            ImGui::DragFloat2("p1", seg.p.data);
            auto p2 = seg.p + seg.dir * seg.len;
            ImGui::DragFloat2("p2", p2.data);
            seg.dir = p2 - seg.p;
            seg.len = seg.dir.Length();
            seg.dir.Normalize();
        }
        popIDs(3);
    }

    void operator()(geom2d::Circle<float>& c) {
        pushIDs(3);
        if (ImGui::CollapsingHeader("Circle")) {
            ImGui::DragFloat2("center", c.center.data);
            ImGui::DragFloat("radius", &c.radius);
        }
        popIDs(3);
    }

    void operator()(geom2d::Ray<float>& r) {
        pushIDs(3);
        if (ImGui::CollapsingHeader("Ray")) {
            ImGui::DragFloat2("point", r.p.data);
            ImGui::DragFloat("direction", r.dir.data);
        }
        popIDs(3);
    }

    void operator()(Polygon<float>& polygon) {
        pushIDs(1);
        if (ImGui::CollapsingHeader("Polygon")) {
            if (ImGui::Button("add vertex")) {
                polygon.push_back(cgmath::Vec2{WindowCenterX, WindowCenterY});
            }
            for (int i = (int)polygon.size() - 1; i >= 0; i--) {
                pushIDs(2);
                ImGui::DragFloat2(("pt" + std::to_string(i)).c_str(),
                                  polygon[i].data);

                ImGui::SameLine();
                if (ImGui::Button("delete")) {
                    polygon.erase(polygon.begin() + i);
                    popIDs(2);
                    continue;
                }

                popIDs(2);
            }
        }
        popIDs(1);
    }

private:
    int id_ = 0;

    void pushIDs(int num) {
        for (int i = 0; i < num; i++) {
            ImGui::PushID(id_++);
        }
    }

    void popIDs(int num) const {
        for (int i = 0; i < num; i++) {
            ImGui::PopID();
        }
    }
};

struct NearestPointCalculer final {
    cgmath::Vec2 operator()(const geom2d::AABB<float>& g,
                            const cgmath::Vec2& pt) const {
        return geom::AABBNearestPt(g, pt);
    }

    cgmath::Vec2 operator()(const geom2d::Circle<float>& g,
                            const cgmath::Vec2& pt) const {
        return geom::CircularNearestPt(g, pt);
    }

    cgmath::Vec2 operator()(const geom2d::Capsule<float>& g,
                            const cgmath::Vec2& pt) const {
        return geom::CapsuleNearestPt(g, pt);
    }

    cgmath::Vec2 operator()(const geom2d::Ray<float>& g,
                            const cgmath::Vec2& pt) const {
        return geom::RayNearestPt(g, pt);
    }

    cgmath::Vec2 operator()(const geom2d::Line<float>& g,
                            const cgmath::Vec2& pt) const {
        return geom::LineNearestPt(g, pt);
    }

    cgmath::Vec2 operator()(const geom2d::Segment<float>& g,
                            const cgmath::Vec2& pt) const {
        return geom::SegNearestPt(g, pt);
    }

    cgmath::Vec2 operator()(const Polygon<float>& g,
                            const cgmath::Vec2& pt) const {
        // TODO: not finish
        return cgmath::Vec2{};
    }
};

struct IntersectCalculer final {
    IntersectCalculer(PolygonIntersectMode mode, Context& ctx)
        : mode_(mode), ctx_(ctx) {}

    bool operator()(const geom2d::Line<float>& l1,
                    geom2d::Line<float>& l2) const {
        return !geom::IsLineParallel(l1, l2);
    }

    bool operator()(const geom2d::AABB<float>& g1,
                    geom2d::AABB<float>& g2) const {
        return geom::IsAABBIntersect(g1, g2);
    }

    bool operator()(const geom2d::Circle<float>& g1,
                    geom2d::Circle<float>& g2) const {
        return geom::IsCircularIntersect(g1, g2);
    }

    bool operator()(const geom2d::Capsule<float>& g1,
                    geom2d::Capsule<float>& g2) const {
        return geom::IsCapsuleIntersect(g1, g2);
    }

    bool operator()(const geom2d::Segment<float>& g1,
                    geom2d::Segment<float>& g2) const {
        return geom2d::IsSegIntersect(g1, g2);
    }

    bool operator()(const geom2d::Ray<float>& g1,
                    geom2d::Ray<float>& g2) const {
        return geom2d::IsRayIntersect(g1, g2);
    }

    bool operator()(const Polygon<float>& g1, const Polygon<float>& g2) const {
        cgmath::Vec2 center;
        for (auto& p : g1) {
            center += p;
        }
        center /= g1.size();

        switch (mode_) {
            case PolygonIntersectMode::GJK:
                return geom2d::Gjk(g1, g2);
            case PolygonIntersectMode::GJKNearest: {
                std::vector<cgmath::Vec2> simplex;
                auto result = geom2d::GjkNearestPt(g1, g2, &simplex);
                if (result.has_value()) {
                    Context::IntersectInfo info;
                    info.p1 = result->first.GetPt(g1);
                    info.p2 = result->second.GetPt(g2);
                    ctx_.intersectResult.push_back(info);
                } else {
                    if (ctx_.useEPA) {
                        if (auto result = geom2d::EPA(simplex, g1, g2);
                            result) {
                            Context::IntersectInfo info;
                            info.center = center;
                            info.mtv = result.value();
                            ctx_.intersectResult.push_back(info);
                        }
                    }
                }
                return !result.has_value();
            }
            case PolygonIntersectMode::SAT: {
                auto result = geom2d::SAT(g1, g2);
                Context::IntersectInfo info;
                if (result) {
                    info.mtv = result.value();
                }
                info.center = center;
                ctx_.intersectResult.push_back(info);
                return result.has_value();
            }
        }
    }

private:
    PolygonIntersectMode mode_;
    Context& ctx_;
};

void RenderGeometrics(gecs::resource<Context> ctx,
                      gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (int i = 0; i < ctx->geoms.size(); i++) {
        const cgmath::Color* color = &GeomColor;
        if (ctx->intersected[i]) {
            color = &ctx->intersectColor;
        }
        std::visit(GeometricRender{renderer.get(), *color}, ctx->geoms[i]);
    }

    for (auto& info : ctx->intersectResult) {
        cgmath::Vec2 ptRect = {5, 5};

        switch (ctx->intersectMode) {
            case PolygonIntersectMode::GJK:
                break;
            case PolygonIntersectMode::GJKNearest:
                // draw nearest pt
                renderer->FillRect({info.p1 - ptRect, ptRect * 2},
                                   {1, 0, 1, 1});
                renderer->FillRect({info.p2 - ptRect, ptRect * 2},
                                   {1, 0, 1, 1});
                renderer->DrawLine(info.p1, info.p2, {1, 0, 1, 1});

                if (ctx->useEPA) {
                    renderer->DrawLine(info.center,
                                       info.center + info.mtv.v * info.mtv.len,
                                       {1, 0, 1, 1});
                }
                break;
            case PolygonIntersectMode::SAT:
                renderer->DrawLine(info.center,
                                   info.center + info.mtv.v * info.mtv.len,
                                   {1, 0, 1, 1});
                break;
        }
    }
}

void ShowGeometricInfos(gecs::resource<gecs::mut<Context>> ctx) {
    for (int i = ctx->geoms.size() - 1; i >= 0; i--) {
        ImGui::PushID(i);
        if (ImGui::Button("delete")) {
            ctx->geoms.erase(ctx->geoms.begin() + i);
            ctx->intersected.erase(ctx->intersected.begin() + i);
            ImGui::PopID();
            continue;
        }
        ImGui::PopID();
        ImGui::SameLine();
        std::visit(GeometricInfoShower{i}, ctx->geoms[i]);
    }
}

void ShowNearestPt(gecs::resource<gecs::mut<Context>> ctx,
                   gecs::resource<Mouse> mouse,
                   gecs::resource<gecs::mut<Renderer2D>> renderer) {
    auto& geoms = ctx->geoms;
    for (auto& geom : geoms) {
        std::visit(
            [&](auto&& g) {
                auto pt = NearestPointCalculer{}(g, mouse->Position());
                renderer->FillRect(
                    cgmath::Rect::FromCenter(pt, cgmath::Vec2{5, 5}),
                    ctx->nearestPtColor);
            },
            geom);
    }
}

void ShowIntersect(gecs::resource<gecs::mut<Context>> ctx,
                   gecs::resource<gecs::mut<Renderer2D>> renderer) {
    auto& geoms = ctx->geoms;
    for (int i = 0; i < (int)geoms.size() - 1; i++) {
        std::visit(
            [&](auto&& g1) {
                for (int j = i + 1; j < geoms.size(); j++) {
                    std::visit(
                        [&](auto&& g2) {
                            using t1 = std::decay_t<decltype(g1)>;
                            using t2 = std::decay_t<decltype(g2)>;
                            if constexpr (std::is_same_v<t1, t2>) {
                                if (IntersectCalculer{ctx->intersectMode,
                                                      ctx.get()}(g1, g2)) {
                                    ctx->intersected[i] =
                                        ctx->intersected[i] || true;
                                    ctx->intersected[j] =
                                        ctx->intersected[j] || true;
                                }
                            }
                        },
                        geoms[j]);
                }
            },
            geoms[i]);
    }
}

void PlaygroundUpdate(gecs::resource<gecs::mut<Context>> ctx,
                      gecs::resource<Mouse> mouse,
                      gecs::resource<gecs::mut<Renderer2D>> renderer) {
    if (ImGui::Begin("console")) {
        ImGui::Checkbox("nearest point", &ctx->showNearestPoint);
        ImGui::ColorEdit3("##color1", ctx->nearestPtColor.data);

        ImGui::Spacing();

        ImGui::Checkbox("intersect point", &ctx->showIntersect);
        ImGui::ColorEdit3("##color2", ctx->intersectColor.data);

        ImGui::Spacing();
        ImGui::Text("intersect type:");
        const char* options[] = {"GJK", "GJK nearest", "SAT"};
        static int selected = 2;
        for (int i = 0; i < 3; i++) {
            ImGui::RadioButton(options[i], &selected, i);
        }

        ctx->intersectMode = static_cast<PolygonIntersectMode>(selected);
        if (selected == 1) {
            ImGui::Checkbox("use EPA", &ctx->useEPA);
        }

        ImGui::Separator();

        ImGui::Text("add gemetrics:");
        for (auto& [label, func] : GemetricAddMethods) {
            if (ImGui::Button(label.data())) {
                func(ctx.get());
                ctx->intersected.resize(ctx->geoms.size());
            }
        }

        ImGui::Separator();
        ShowGeometricInfos(ctx);

        if (ctx->showNearestPoint) {
            ShowNearestPt(ctx, mouse, renderer);
        }

        if (ctx->showIntersect) {
            ShowIntersect(ctx, renderer);
        }
    }
    ImGui::End();
}

void ResetContextState(gecs::resource<gecs::mut<Context>> ctx) {
    ctx->intersected.assign(ctx->intersected.size(), false);
    ctx->intersectResult.clear();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    ProjectInitInfo initInfo;
    initInfo.windowData.title = "geometric playground";
    initInfo.windowData.size.Set(WindowWidth, WindowHeight);
    InitSystem(world, initInfo, reg.commands());

    reg.commands().emplace_resource<ProjectInitInfo>(std::move(initInfo));

    reg.regist_startup_system<PlaygroundStartup>()
        .regist_startup_system<ImGuiInit>()
        .regist_update_system<ImGuiStart>()
        .regist_update_system<PlaygroundUpdate>()
        .regist_update_system<RenderGeometrics>()
        .regist_update_system<ImGuiEnd>()
        .regist_update_system<ResetContextState>();
}
