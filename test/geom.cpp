#include "geom/basic_geom.hpp"
#include "geom/geom2d.hpp"
#include "geom/geom3d.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace nickel;

TEST_CASE("nearest point") {
    SECTION("line") {
        auto l = geom2d::Line<float>::FromDir(
            cgmath::Vec2{2, 2}, cgmath::Normalize(cgmath::Vec2{1, 1}));
        REQUIRE(geom::LineNearestPt(l, cgmath::Vec2{3, 3}) ==
                cgmath::Vec2{3, 3});
        REQUIRE(geom::LineNearestPt(l, cgmath::Vec2{4, 1}) ==
                cgmath::Vec2{2.5, 2.5});
    }

    SECTION("segment") {
        auto s = geom2d::Segment<float>::FromPts(cgmath::Vec2{1, 1},
                                                 cgmath::Vec2{7, 5});
        REQUIRE(geom::IsSamePt(geom::SegNearestPt(s, cgmath::Vec2{4, 1}),
                               cgmath::Vec2{3.07692f, 2.38462f}));
        REQUIRE(geom::IsSamePt(geom::SegNearestPt(s, cgmath::Vec2{5, 6}),
                               cgmath::Vec2{6.08f, 4.38f}));
        REQUIRE(geom::SegNearestPt(s, cgmath::Vec2{8, 7}) ==
                cgmath::Vec2{7, 5});
        REQUIRE(geom::SegNearestPt(s, cgmath::Vec2{0, -3}) ==
                cgmath::Vec2{1, 1});
    }

    SECTION("ray") {
        auto ray = geom2d::Ray<float>::FromDir(
            cgmath::Vec2{1, 1}, cgmath::Normalize(cgmath::Vec2{6, 4}));
        REQUIRE(geom::IsSamePt(geom::RayNearestPt(ray, cgmath::Vec2{4, 1}),
                               cgmath::Vec2{3.07692f, 2.38462f}));
        REQUIRE(geom::IsSamePt(geom::RayNearestPt(ray, cgmath::Vec2{5, 6}),
                               cgmath::Vec2{6.08f, 4.38f}));
        REQUIRE(geom::IsSamePt(geom::RayNearestPt(ray, cgmath::Vec2{8, 7}),
                               cgmath::Vec2{8.62f, 6.08f}));
        REQUIRE(geom::RayNearestPt(ray, cgmath::Vec2{0, -3}) ==
                cgmath::Vec2{1, 1});
    }

    SECTION("circle") {
        auto c = geom2d::Circle<float>::Create(cgmath::Vec2{1, 1}, 3.5);
        REQUIRE(geom::CircularNearestPt(c, cgmath::Vec2{1, 1}) ==
                cgmath::Vec2{1, 1});
        REQUIRE(geom::CircularNearestPt(c, cgmath::Vec2{3, 1}) ==
                cgmath::Vec2{3, 1});
        REQUIRE(geom::CircularNearestPt(c, cgmath::Vec2{5, 1}) ==
                cgmath::Vec2{4.5f, 1});
        REQUIRE(geom::IsSamePt(geom::CircularNearestPt(c, cgmath::Vec2{6, 4}),
                               cgmath::Vec2{4, 2.8f}));
    }

    SECTION("capsule") {
        auto c = geom2d::Capsule<float>::Create(cgmath::Vec2{-3, 2},
                                                cgmath::Vec2{4, 6}, 3.0);
        REQUIRE(geom::IsSamePt(geom::CapsuleNearestPt(c, cgmath::Vec2{8, 5}),
                               cgmath::Vec2{6.91, 5.27}));
        REQUIRE(geom::IsSamePt(
            geom::CapsuleNearestPt(c, cgmath::Vec2{-6.67, -1.17}),
            cgmath::Vec2{-5.27, 0.04}));
        REQUIRE(geom::IsSamePt(
            geom::CapsuleNearestPt(c, cgmath::Vec2{-6.67, -1.17}),
            cgmath::Vec2{-5.27, 0.04}));
        REQUIRE(
            geom::IsSamePt(geom::CapsuleNearestPt(c, cgmath::Vec2{-2.65, 8.99}),
                           cgmath::Vec2{-1.21, 6.48}));
    }
}

TEST_CASE("intersection check") {
    SECTION("line-line") {
        auto l1 = geom2d::Line<float>::FromPts(cgmath::Vec2{-3, 3},
                                               cgmath::Vec2{2, -1});
        auto l2 = geom2d::Line<float>::FromPts(cgmath::Vec2{-2, 1},
                                               cgmath::Vec2{1, 3});
        REQUIRE(geom2d::IsLineIntersect(l1, l2));

        auto l3 = geom2d::Line<float>::FromPts(cgmath::Vec2{-4, 1},
                                               cgmath::Vec2{-1, 3});
        REQUIRE_FALSE(geom2d::IsLineIntersect(l3, l2));

        REQUIRE_FALSE(geom2d::IsLineIntersect(
            geom2d::Line<float>::FromDir(cgmath::Vec2{0, 0},
                                         cgmath::Vec2{0, 1}),
            geom2d::Line<float>::FromDir(cgmath::Vec2{4, 0},
                                         cgmath::Vec2{0, -1})));
    }

    SECTION("seg-seg") {
        auto s1 = geom2d::Segment<float>::FromPts(cgmath::Vec2{-2, 3},
                                                  cgmath::Vec2{1, 1});
        auto s2 = geom2d::Segment<float>::FromPts(cgmath::Vec2{-2, 1},
                                                  cgmath::Vec2{0, 5});

        REQUIRE(geom2d::IsSegIntersect(s1, s2));

        auto s3 = geom2d::Segment<float>::FromPts(cgmath::Vec2{0, 0},
                                                  cgmath::Vec2{-4, 5});
        REQUIRE_FALSE(geom2d::IsSegIntersect(s1, s3));
    }

    SECTION("ray-ray") {
        REQUIRE(geom2d::IsRayIntersect(
            geom2d::Ray<float>::FromPts(cgmath::Vec2{-2, 3},
                                        cgmath::Vec2{1.94, 5.8}),
            geom2d::Ray<float>::FromPts(cgmath::Vec2{-1, 0.86},
                                        cgmath::Vec2{-1.38, 4.94})));

        REQUIRE_FALSE(geom2d::IsRayIntersect(
            geom2d::Ray<float>::FromPts(cgmath::Vec2{1, 1},
                                        cgmath::Vec2{1.94, 5.8}),
            geom2d::Ray<float>::FromPts(cgmath::Vec2{-1, 0.86},
                                        cgmath::Vec2{-1.38, 4.94})));

        REQUIRE_FALSE(geom2d::IsRayIntersect(
            geom2d::Ray<float>::FromPts(cgmath::Vec2{0.88, 0.36},
                                        cgmath::Vec2{1.94, 5.8}),
            geom2d::Ray<float>::FromPts(cgmath::Vec2{1, -2},
                                        cgmath::Vec2{-1.38, 4.94})));

        REQUIRE_FALSE(geom2d::IsRayIntersect(
            geom2d::Ray<float>::FromPts(cgmath::Vec2{0.88, 0.36},
                                        cgmath::Vec2{1.4, -2.9}),
            geom2d::Ray<float>::FromPts(cgmath::Vec2{1, -2},
                                        cgmath::Vec2{-1.38, 4.94})));
    }
}

TEST_CASE("misc") {
    SECTION("3D lines minimal segment") {
        auto l1 = geom3d::Line<float>::FromPts(cgmath::Vec3{3.87145, -0.16841, 0},
                                         cgmath::Vec3{-0.46877, -0.57715, 3});
        auto l2 = geom3d::Line<float>::FromPts(cgmath::Vec3{1.01, -2.83, 0},
                                         cgmath::Vec3{-4.12, 4.54, 0});
        auto [param1, param2] = geom3d::MinSegBetweenLines(l1, l2);
        REQUIRE(cgmath::IsSameValue((l1.p + l1.dir * param1 - (l2.p + l2.dir * param2)).Length(), 2.3991f, 0.0001f));

        l1 = geom3d::Line<float>::FromPts(cgmath::Vec3{3.87145, -0.16841, 0},
                                         cgmath::Vec3{-0.46877, -0.57715, 3});
        l2 = geom3d::Line<float>::FromDir(cgmath::Vec3{-1.7, 0.06, 1.14},
                                         cgmath::Normalize(cgmath::Vec3{-1.59, -0.15, 1.1}));
        auto [param3, param4] = geom3d::MinSegBetweenLines(l1, l2);
                                        
        REQUIRE(cgmath::IsSameValue((l1.p + l1.dir * param3 - (l2.p + l2.dir * param4)).Length(), 2.32013f, 0.01f));
    }
}

using geom_type = std::vector<cgmath::Vec2>;

bool DoGjk(const geom_type& g1, const geom_type& g2, const cgmath::Vec2& pos1, const cgmath::Vec2& pos2) {
    geom_type newg1 = g1, newg2 = g2;
    for (auto& e : newg1) {
        e += pos1;
    }

    for (auto& e : newg2) {
        e += pos2;
    }

    return geom2d::Gjk(newg1, newg2);
}

TEST_CASE("gjk", "[2D]") {
    SECTION("triangle") {
        geom_type geom = {
            {-5,  5},
            {5,  -5},
            {0, 5},
        };

        REQUIRE(DoGjk(geom, geom, cgmath::Vec2{0, 0}, cgmath::Vec2{2, 2}));
        REQUIRE_FALSE(
            DoGjk(geom, geom, cgmath::Vec2{0, 0}, cgmath::Vec2{100, 0}));
    }

    SECTION("quadrilateral") {
        geom_type geom = {
            {-2,  7},
            {-6,  3},
            {-3, -3},
            { 4, -4}
        };

        REQUIRE(DoGjk(geom, geom, cgmath::Vec2{0, 0}, cgmath::Vec2{2, 2}));
        REQUIRE_FALSE(DoGjk(geom, geom, cgmath::Vec2{0, 0}, cgmath::Vec2{200, 2}));
    }
}