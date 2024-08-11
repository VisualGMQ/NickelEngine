#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "common/math/math.hpp"
#include "common/math/matrix.hpp"
#include "common/math/smatrix.hpp"


#define FLT_EQ(a, b) (std::abs(a - b) <= std::numeric_limits<float>::epsilon())

using namespace nickel;

TEST_CASE("matrix") {
    // clang-format off
    auto m1 = DMat::FromRow(2, 3,
                                1, 2,
                                3, 4,
                                5, 6);
    // clang-format on
    REQUIRE(m1[0][0] == 1);
    REQUIRE(m1[0][1] == 3);
    REQUIRE(m1[0][2] == 5);
    REQUIRE(m1[1][0] == 2);
    REQUIRE(m1[1][1] == 4);
    REQUIRE(m1[1][2] == 6);

    // clang-format off
    auto m2 = DMat::FromCol(3, 2,
        1, 2,
        3, 4,
        5, 6);
    // clang-format on
    REQUIRE(m2[0][0] == 1);
    REQUIRE(m2[0][1] == 2);
    REQUIRE(m2[1][0] == 3);
    REQUIRE(m2[1][1] == 4);
    REQUIRE(m2[2][0] == 5);
    REQUIRE(m2[2][1] == 6);
}

TEST_CASE("smatrix") {
    SECTION("arbitary matrix") {
        // clang-format off
        auto m1 = SMatrix<float, 2, 3>::FromCol(
            1, 2, 3,
            4, 5, 6
        );
        // clang-format on
        REQUIRE(m1[0][0] == 1);
        REQUIRE(m1[0][1] == 2);
        REQUIRE(m1[0][2] == 3);
        REQUIRE(m1[1][0] == 4);
        REQUIRE(m1[1][1] == 5);
        REQUIRE(m1[1][2] == 6);

        // clang-format off
        auto m2 = SMatrix<float, 3, 2>::FromCol(
            1, 2,
            3, 4,
            5, 6
        );
        // clang-format on
        REQUIRE(m2[0][0] == 1);
        REQUIRE(m2[0][1] == 2);
        REQUIRE(m2[1][0] == 3);
        REQUIRE(m2[1][1] == 4);
        REQUIRE(m2[2][0] == 5);
        REQUIRE(m2[2][1] == 6);
    }
}

TEST_CASE("matrix view") {
    SECTION("matrix") {
        // clang-format off
        auto m = DMat::FromRow(2, 3,
                                  1, 2,
                                  3, 4,
                                  5, 6);
        // clang-format on

        auto v1 = SubMatView(m, 0, 2, 1, 2);
        REQUIRE(v1[0][0] == 3);
        REQUIRE(v1[0][1] == 5);
        REQUIRE(v1[1][0] == 4);
        REQUIRE(v1[1][1] == 6);
    }
}

TEST_CASE("dynamic math") {
    SECTION("matrix multiple") {
        // clang-format off
        auto m1 = DMat::FromRow(2, 3,
                                1, 2,
                                3, 4,
                                5, 6);
        auto m2 = DMat::FromRow(2, 2,
                                    1, 2,
                                    3, 4);
        // clang-format on

        auto result = m1 * m2;
        // clang-format off
        REQUIRE(result == DMat::FromRow(2, 3,
            7, 10, 15,
            22, 23, 34));
        // clang-format on
    }

    SECTION("matrix determination") {
        // clang-format off
        auto m1 = DMat::FromRow(1, 1, 3);
        auto m2 = DMat::FromRow(2, 2,
            1, 2,
            3, 4);
        auto m3 = DMat::FromRow(3, 3,
            2, 3, 1,
            6, 9, 5,
            7, 2, 10);
        auto m4 = DMat::FromRow(4, 4,
            7, 8, 9, 1,
            5, 11, 22, 3,
            23, 56, 98, 4,
            5, 5, 2, 1);
        // clang-format on
        REQUIRE(Det(m1) == 3);
        REQUIRE(Det(m2) == -2);
        REQUIRE(Det(m3) == 34);
        REQUIRE(Det(m4) == -1464);
    }

    SECTION("matrix cofactor") {
        auto m3 = DMat::FromRow(3, 3, 2, 3, 1, 6, 9, 5, 7, 2, 10);
        REQUIRE(Cofactor(m3, 0, 0) == DMat::FromRow(2, 2, 9, 5, 2, 10));
        REQUIRE(Cofactor(m3, 1, 0) == DMat::FromRow(2, 2, 6, 5, 7, 10));
        REQUIRE(Cofactor(m3, 2, 0) == DMat::FromRow(2, 2, 6, 9, 7, 2));
    }

    SECTION("linear equations solver") {
        // clang-format off
        auto m = DMat::FromRow(4, 3,
            1, 3, 4, 5,
            1, 4, 7, 3,
            9, 3, 2, 2);
        auto result = Solve(SubMatView{m});
        REQUIRE(result.has_value());

        REQUIRE(result.value() ==
                DMat::FromRow(1, 3,
                    -0.973684,
                    5.18421,
                    -2.39474));
        // clang-format on
    }
}

TEST_CASE("units", "[angle]") {
    SECTION("create") {
        Radians r1 = 12.0f;
        Radians r2 = -12;

        REQUIRE(static_cast<float>(r1) == 12.0f);
        REQUIRE(static_cast<float>(r2) == -12);

        Degrees d1 = 39.0f;
        Degrees d2 = -22.0f;

        REQUIRE(static_cast<float>(d1) == 39.0f);
        REQUIRE(static_cast<float>(d2) == -22);

        REQUIRE(123_rad == Radians(123));
        REQUIRE(-123_rad == Radians(-123));

        REQUIRE(123.9_deg == Degrees(123.9f));
        REQUIRE(-123.9_deg == Degrees(-123.9f));
        REQUIRE(123_deg == Degrees(123));
        REQUIRE(-123_deg == Degrees(-123));
    }

    SECTION("conversion") {
        Degrees deg = 30.0_deg;
        Radians rad = deg;
        REQUIRE(rad == Radians(30.0f * GenericPI<float> / 180.0f));

        rad = 30.0f * GenericPI<float> / 180.0f;
        deg = rad;
        REQUIRE(deg == Degrees(30.0f));
    }

    SECTION("compare") {
        Radians rad = GenericPI<float>;

        REQUIRE(rad > 170_deg);
        REQUIRE(rad >= 180_deg);
        REQUIRE(rad == 180_deg);
        REQUIRE_FALSE(rad > 200_deg);
        REQUIRE_FALSE(rad >= 200_deg);

        REQUIRE_FALSE(rad < 170_deg);
        REQUIRE(rad <= 180_deg);
        REQUIRE_FALSE(rad != 180_deg);
        REQUIRE(rad < 200_deg);
        REQUIRE(rad <= 200_deg);

        REQUIRE(170_deg < rad);
        REQUIRE(180_deg <= rad);
        REQUIRE(180_deg == rad);
        REQUIRE_FALSE(200_deg < rad);
        REQUIRE_FALSE(200_deg <= rad);

        REQUIRE_FALSE(170_deg > rad);
        REQUIRE(180_deg >= rad);
        REQUIRE_FALSE(180_deg != rad);
        REQUIRE(200_deg > rad);
        REQUIRE(200_deg >= rad);
    }
}

TEST_CASE("vectors can be add, sub, mul and divide", "[vector2]") {
    Vec2 v1{1, 2};
    Vec2 v2{2, 3};

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
        REQUIRE(result.x == 0.5f);
        REQUIRE(result.y == 2.0f / 3.0f);
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
        auto result = Dot(Vec2{1, 2}, Vec2{2, 3});
        REQUIRE(result == 8);
    }
}

TEST_CASE("Vec2 and Vec3 can be cross", "[vector]") {
    SECTION("Vec2 cross") {
        auto result = Cross(Vec2{1, 2}, Vec2{2, 3});
        REQUIRE(result == -1);
    }

    SECTION("Vec3 cross") {
        auto result = Cross(Vec3{1, 2, 3}, Vec3{4, 5, 6});
        REQUIRE(result.x == -3);
        REQUIRE(result.y == 6);
        REQUIRE(result.z == -3);
    }
}

TEST_CASE("vectors can get length and normalize", "[vector]") {
    SECTION("vector length") {
        Vec2 v1{1, 2};
        Vec2 v2{2, 3};
        REQUIRE(LengthSqrd(v1) == 5);
        REQUIRE(LengthSqrd(v2) == 13);
        REQUIRE(FLT_EQ(Length(v1), std::sqrt(5)));
        REQUIRE(FLT_EQ(Length(v2), std::sqrt(13)));
    }

    SECTION("vector normalize") {
        Vec2 v1{1, 2};
        float len = Length(v1);
        v1 = Normalize(v1);
        REQUIRE(v1.x == 1.0f / len);
        REQUIRE(v1.y == 2.0f / len);

        auto v2 = Normalize(Vec2{1, 2});
        REQUIRE(v2.x == 1.0f / len);
        REQUIRE(v2.y == 2.0f / len);
    }
}

TEST_CASE("vectors can be assigned", "[vector]") {
    Vec2 v1{1, 2};
    Vec2 v2 = v1;
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
    Mat22 m1 = Mat22::FromRow(1, 2, 3, 4);
    /*
    [5, 6]
    [7, 8]
    */
    Mat22 m2 = Mat22::FromCol(5, 7, 6, 8);

    SECTION("matrix init") {
        REQUIRE(m1[0][0] == 1);
        REQUIRE(m1[1][0] == 2);
        REQUIRE(m1[0][1] == 3);
        REQUIRE(m1[1][1] == 4);

        REQUIRE(m2[0][0] == 5);
        REQUIRE(m2[1][0] == 6);
        REQUIRE(m2[0][1] == 7);
        REQUIRE(m2[1][1] == 8);
    }

    SECTION("matrix add") {
        auto m = m1 + m2;
        REQUIRE(m[0][0] == 6);
        REQUIRE(m[1][0] == 8);
        REQUIRE(m[0][1] == 10);
        REQUIRE(m[1][1] == 12);
    }

    SECTION("matrix sub") {
        auto m = m1 - m2;
        REQUIRE(m[0][0] == -4);
        REQUIRE(m[1][0] == -4);
        REQUIRE(m[0][1] == -4);
        REQUIRE(m[1][1] == -4);
    }

    SECTION("matrix mul") {
        auto m = MulEach(m1, m2);
        REQUIRE(m[0][0] == 5);
        REQUIRE(m[1][0] == 12);
        REQUIRE(m[0][1] == 21);
        REQUIRE(m[1][1] == 32);
    }

    SECTION("matrix div") {
        auto m = DivEach(m1, m2);
        REQUIRE(m[0][0] == 1.0f / 5.0f);
        REQUIRE(m[1][0] == 2.0f / 6.0f);
        REQUIRE(m[0][1] == 3.0f / 7.0f);
        REQUIRE(m[1][1] == 4.0f / 8.0f);
    }

    SECTION("matrix mul scalar") {
        auto m = m1 * 2.0;
        REQUIRE(m[0][0] == 2);
        REQUIRE(m[1][0] == 4);
        REQUIRE(m[0][1] == 6);
        REQUIRE(m[1][1] == 8);

        m = 2.0 * m1;
        REQUIRE(m[0][0] == 2);
        REQUIRE(m[1][0] == 4);
        REQUIRE(m[0][1] == 6);
        REQUIRE(m[1][1] == 8);
    }

    SECTION("matrix multiply matrix") {
        auto m = m1 * m2;
        REQUIRE(m[0][0] == 19);
        REQUIRE(m[1][0] == 22);
        REQUIRE(m[0][1] == 43);
        REQUIRE(m[1][1] == 50);

        auto m3 = SMatrix<float, 2, 3>::FromRow(
            1, 2,
            3, 4,
            5, 6
        );

        auto m4 = SMatrix<float, 3, 2>::FromRow(
            1, 2, 3,
            4, 5, 6
        );

        auto result = m3 * m4;
        REQUIRE(result.ColNum() == 3);
        REQUIRE(result.RowNum() == 3);
        REQUIRE(result[0][0] == 9);
        REQUIRE(result[1][0] == 12);
        REQUIRE(result[2][0] == 15);
        REQUIRE(result[0][1] == 19);
        REQUIRE(result[1][1] == 26);
        REQUIRE(result[2][1] == 33);
        REQUIRE(result[0][2] == 29);
        REQUIRE(result[1][2] == 40);
        REQUIRE(result[2][2] == 51);
    }

    SECTION("matrix multiple vector") {
        auto m = Mat22::FromRow(1, 2, 3, 4);
        Vec2 v{6, 7};
        auto result = m * v;

        REQUIRE(result.x == 20);
        REQUIRE(result.y == 46);
    }
}
