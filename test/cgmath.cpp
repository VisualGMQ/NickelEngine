// Copyright 2023 VisualGMQ
// a simple math library for computer graphics and computational geomentry

#include <limits>

#define CGMATH_NUMERIC_TYPE double
#include "core/cgmath.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace nickel;

#define FLT_EQ(a, b) (std::abs(a - b) <= std::numeric_limits<CGMATH_NUMERIC_TYPE>::epsilon())

TEST_CASE("vectors can be add, sub, mul and divide", "[vector2]") {
    cgmath::Vec2 v1{1, 2};
    cgmath::Vec2 v2{2, 3};

    SECTION("vector add") {
        auto result = v1 + v2;
        REQUIRE(result.x == 3);
        REQUIRE(result.y == 5);
    }

    SECTION("vector sub") {
        auto result = v1 - v2;
        REQUIRE(result.x == -1);
        REQUIRE(result.y == -1);
    }

    SECTION("vector mul") {
        auto result = v1 * v2;
        REQUIRE(result.x == 2);
        REQUIRE(result.y == 6);
    }

    SECTION("vector div") {
        auto result = v1 / v2;
        REQUIRE(result.x == 0.5);
        REQUIRE(result.y == 2.0 / 3.0);
    }

    SECTION("vector mul scalar") {
        auto result = v1 * 0.5;
        REQUIRE(result.x == 0.5);
        REQUIRE(result.y == 1);
        result = 0.5 * v1;
        REQUIRE(result.x == 0.5);
        REQUIRE(result.y == 1);
    }
}

TEST_CASE("vectors can be dot", "[vector]") {
    SECTION("vector dot") {
        auto result = cgmath::Vec2{1, 2}.Dot(cgmath::Vec2{2, 3});
        REQUIRE(result == 8);
    }
}

TEST_CASE("Vec2 and Vec3 can be cross", "[vector]") {
    SECTION("Vec2 cross") {
        auto result = cgmath::Vec2{1, 2}.Cross(cgmath::Vec2{2, 3});
        REQUIRE(result == -1);
    }

    SECTION("Vec3 cross") {
        auto result = cgmath::Vec3{1, 2, 3}.Cross(cgmath::Vec3{4, 5, 6});
        REQUIRE(result.x == -3);
        REQUIRE(result.y == 6);
        REQUIRE(result.z == -3);
    }
}

TEST_CASE("vectors can get length and normalize", "[vector]") {
    SECTION("vector length") {
        cgmath::Vec2 v1{1, 2};
        cgmath::Vec2 v2{2, 3};
        REQUIRE(v1.LengthSqrd() == 5);
        REQUIRE(v2.LengthSqrd() == 13);
        REQUIRE(FLT_EQ(v1.Length(), std::sqrt(5)));
        REQUIRE(FLT_EQ(v2.Length(), std::sqrt(13)));
    }

    SECTION("vector normalize") {
        cgmath::Vec2 v1{1, 2};
        double len = v1.Length();
        v1.Normalize();
        REQUIRE(v1.x == 1 / len);
        REQUIRE(v1.y == 2 / len);

        auto v2 = cgmath::Normalize(cgmath::Vec2{1, 2});
        REQUIRE(v2.x == 1 / len);
        REQUIRE(v2.y == 2 / len);
    }
}

TEST_CASE("vectors can be assigned", "[vector]") {
    cgmath::Vec2 v1{1, 2};
    cgmath::Vec2 v2 = v1;
    v1.x = 3;
    REQUIRE(v1.x == 3);
    REQUIRE(v2.x == 1);
    v2 = v1;
    v1.x = 6;
    REQUIRE(v2.x == 3);
    REQUIRE(v1.x == 6);
}

TEST_CASE("matrix can be add, sub, multiply and divide", "[matrix]") {
    /*
    [1, 2]
    [3, 4]
    */
    cgmath::Mat22 m1 = cgmath::Mat22::FromRow({1, 2,
                                               3, 4});
    /*
    [5, 6]
    [7, 8]
    */
    cgmath::Mat22 m2 = cgmath::Mat22::FromCol({cgmath::Vec2{5, 7},
                                               cgmath::Vec2{6, 8}});

    SECTION("matrix init") {
        REQUIRE(m1.Get(0, 0) == 1);
        REQUIRE(m1.Get(1, 0) == 2);
        REQUIRE(m1.Get(0, 1) == 3);
        REQUIRE(m1.Get(1, 1) == 4);

        REQUIRE(m2.Get(0, 0) == 5);
        REQUIRE(m2.Get(1, 0) == 6);
        REQUIRE(m2.Get(0, 1) == 7);
        REQUIRE(m2.Get(1, 1) == 8);
    }

    SECTION("matrix add") {
        auto m = m1 + m2;
        REQUIRE(m.Get(0, 0) == 6);
        REQUIRE(m.Get(1, 0) == 8);
        REQUIRE(m.Get(0, 1) == 10);
        REQUIRE(m.Get(1, 1) == 12);
    }

    SECTION("matrix sub") {
        auto m = m1 - m2;
        REQUIRE(m.Get(0, 0) == -4);
        REQUIRE(m.Get(1, 0) == -4);
        REQUIRE(m.Get(0, 1) == -4);
        REQUIRE(m.Get(1, 1) == -4);
    }

    SECTION("matrix mul") {
        auto m = MulEach(m1, m2);
        REQUIRE(m.Get(0, 0) == 5);
        REQUIRE(m.Get(1, 0) == 12);
        REQUIRE(m.Get(0, 1) == 21);
        REQUIRE(m.Get(1, 1) == 32);
    }

    SECTION("matrix div") {
        auto m = DivEach(m1, m2);
        REQUIRE(m.Get(0, 0) == 1.0 / 5.0);
        REQUIRE(m.Get(1, 0) == 2.0 / 6.0);
        REQUIRE(m.Get(0, 1) == 3.0 / 7.0);
        REQUIRE(m.Get(1, 1) == 4.0 / 8.0);
    }

    SECTION("matrix mul scalar") {
        auto m = m1 * 2.0;
        REQUIRE(m.Get(0, 0) == 2);
        REQUIRE(m.Get(1, 0) == 4);
        REQUIRE(m.Get(0, 1) == 6);
        REQUIRE(m.Get(1, 1) == 8);

        m = 2.0 * m1;
        REQUIRE(m.Get(0, 0) == 2);
        REQUIRE(m.Get(1, 0) == 4);
        REQUIRE(m.Get(0, 1) == 6);
        REQUIRE(m.Get(1, 1) == 8);
    }

    SECTION("matrix multiply matrix") {
        auto m = m1 * m2;
        REQUIRE(m.Get(0, 0) == 19);
        REQUIRE(m.Get(1, 0) == 22);
        REQUIRE(m.Get(0, 1) == 43);
        REQUIRE(m.Get(1, 1) == 50);

        auto m3 = cgmath::Mat<float, 2, 3>::FromRow({
            1, 2,
            3, 4,
            5, 6,
        });

        auto m4 = cgmath::Mat<float, 3, 2>::FromRow({
            1, 2, 3,
            4, 5, 6,
        });

        auto result = m3 * m4;
        REQUIRE(result.W() == 3);
        REQUIRE(result.H() == 3);
        REQUIRE(result.Get(0, 0) == 9);
        REQUIRE(result.Get(1, 0) == 12);
        REQUIRE(result.Get(2, 0) == 15);
        REQUIRE(result.Get(0, 1) == 19);
        REQUIRE(result.Get(1, 1) == 26);
        REQUIRE(result.Get(2, 1) == 33);
        REQUIRE(result.Get(0, 2) == 29);
        REQUIRE(result.Get(1, 2) == 40);
        REQUIRE(result.Get(2, 2) == 51);
    }

    SECTION("matrix multiple vector") {
        auto m = cgmath::Mat22::FromRow({1, 2,
                                        3, 4});
        cgmath::Vec2 v{6, 7};
        auto result = m * v;

        REQUIRE(result.x == 20);
        REQUIRE(result.y == 46);
    }
}
