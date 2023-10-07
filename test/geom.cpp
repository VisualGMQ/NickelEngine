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
        REQUIRE(geom::IsSamePt(geom::CapsuleNearestPt(c, cgmath::Vec2{-6.67, -1.17}),
                               cgmath::Vec2{-5.27, 0.04}));
        REQUIRE(geom::IsSamePt(geom::CapsuleNearestPt(c, cgmath::Vec2{-6.67, -1.17}),
                               cgmath::Vec2{-5.27, 0.04}));
        REQUIRE(geom::IsSamePt(geom::CapsuleNearestPt(c, cgmath::Vec2{-2.65, 8.99}),
                               cgmath::Vec2{-1.21, 6.48}));
    }
}

TEST_CASE("intersect") {
    SECTION("line vs line") {
        // TODO: not finish
    }
}