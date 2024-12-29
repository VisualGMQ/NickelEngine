#include "catch2/catch_test_macros.hpp"
#include "nickel/common/math/algorithm.hpp"
#include "nickel/common/math/smatrix.hpp"

using namespace nickel;

TEST_CASE("vector") {
    SECTION("initialize") {
        SVector<float, 2> v1{1, 2};
        REQUIRE(v1.x == 1);
        REQUIRE(v1.y == 2);

        SVector<float, 2> v2{3};
        REQUIRE(v2.x == 3);
        REQUIRE(v2.y == 3);

        SVector<float, 2> v3;
        REQUIRE(v3.x == 0);
        REQUIRE(v3.y == 0);

        SVector<float, 3> v4{1, 3, 5};
        REQUIRE(v4.x == 1);
        REQUIRE(v4.y == 3);
        REQUIRE(v4.z == 5);

        SVector<float, 4> v5{1, 3, 5, 9};
        REQUIRE(v5.x == 1);
        REQUIRE(v5.y == 3);
        REQUIRE(v5.z == 5);
        REQUIRE(v5.w == 9);
    }

    SVector<float, 2> v1{1, 2};
    SECTION("swizzle") {
        REQUIRE(v1.w == v1.x);
        REQUIRE(v1.h == v1.y);
        REQUIRE(v1.s == v1.x);
        REQUIRE(v1.r == v1.y);
        REQUIRE(v1.x == v1[0]);
        REQUIRE(v1.y == v1[1]);

        SVector<float, 3> v2{1, 3, 5};
        REQUIRE(v2.x == v2[0]);
        REQUIRE(v2.y == v2[1]);
        REQUIRE(v2.z == v2[2]);
        REQUIRE(v2.w == v2[0]);
        REQUIRE(v2.h == v2[1]);
        REQUIRE(v2.l == v2[2]);
        REQUIRE(v2.s == v2[0]);
        REQUIRE(v2.r == v2[1]);
        REQUIRE(v2.t == v2[2]);

        SVector<float, 4> v3{1, 3, 5, 9};
        REQUIRE(v3.x == v3[0]);
        REQUIRE(v3.y == v3[1]);
        REQUIRE(v3.z == v3[2]);
        REQUIRE(v3.w == v3[3]);
        REQUIRE(v3.r == v3[0]);
        REQUIRE(v3.g == v3[1]);
        REQUIRE(v3.b == v3[2]);
        REQUIRE(v3.a == v3[3]);
    }

    SECTION("compare") {
        REQUIRE(v1 == SVector<float, 2>{1, 2});
        REQUIRE(v1 != SVector<float, 2>{2, 2});
    }

    SVector<float, 2> v2{3, 5};

    SECTION("operations") {
        auto plus = v1 + v2;
        REQUIRE(plus.x == 4.0f);
        REQUIRE(plus.y == 7.f);

        auto minus = v1 - v2;
        REQUIRE(minus.x == -2.f);
        REQUIRE(minus.y == -3.f);

        auto mul = v1 * v2;
        REQUIRE(mul.x == 3.f);
        REQUIRE(mul.y == 10.f);

        auto div = v1 / v2;
        REQUIRE(div.x == 1.0f / 3.0f);
        REQUIRE(div.y == 0.4f);

        auto neg = -v1;
        REQUIRE(neg.x == -1.f);
        REQUIRE(neg.y == -2.f);

        float dot = Dot(v1, v2);
        REQUIRE(dot == 13.0f);

        float cross = Cross(v1, v2);
        REQUIRE(cross == -1.0f);

        SVector<float, 3> v3{1, 2, 3};
        SVector<float, 3> v4{9, 7, 1};
        REQUIRE(Cross(v3, v4) == SVector<float, 3>{-19, 26, -11});
    }
}

TEST_CASE("matrix") {
    SECTION("initialize") {
        {
            SMatrix<float, 2, 2> mat{};
            REQUIRE(mat[0][0] == 0);
            REQUIRE(mat[0][1] == 0);
            REQUIRE(mat[1][0] == 0);
            REQUIRE(mat[1][1] == 0);
        }
        {
            auto mat = SMatrix<float, 2, 2>::FromCol(1, 2, 3, 4);
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 2);
            REQUIRE(mat[1][0] == 3);
            REQUIRE(mat[1][1] == 4);
        }
        {
            auto mat = SMatrix<float, 2, 2>::FromRow(1, 2, 3, 4);
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 3);
            REQUIRE(mat[1][0] == 2);
            REQUIRE(mat[1][1] == 4);
        }
        {
            auto mat = SMatrix<float, 2, 2>::Ones();
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 1);
            REQUIRE(mat[1][0] == 1);
            REQUIRE(mat[1][1] == 1);
        }
        {
            auto mat = SMatrix<float, 2, 2>::Identity();
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 0);
            REQUIRE(mat[1][0] == 0);
            REQUIRE(mat[1][1] == 1);
        }
        {
            auto mat = SMatrix<float, 2, 2>::FillWith(8);
            REQUIRE(mat[0][0] == 8);
            REQUIRE(mat[0][1] == 8);
            REQUIRE(mat[1][0] == 8);
            REQUIRE(mat[1][1] == 8);
        }
    }

    SECTION("compare") {
        auto m = SMatrix<float, 2, 2>::FromCol(1, 2, 3, 4);
        REQUIRE(m == SMatrix<float, 2, 2>::FromCol(1, 2, 3, 4));
        REQUIRE(m != SMatrix<float, 2, 2>::FromRow(1, 2, 3, 4));
    }

    SECTION("operations") {
        auto m1 = SMatrix<float, 2, 2>::FromCol(1, 2, 3, 4);
        auto m2 = SMatrix<float, 2, 2>::FromCol(4, 6, 8, 1);
        REQUIRE(m1 + m2 == SMatrix<float, 2, 2>::FromCol(5, 8, 11, 5));
        REQUIRE(m1 - m2 == SMatrix<float, 2, 2>::FromCol(-3, -4, -5, 3));
        REQUIRE(-m1 == SMatrix<float, 2, 2>::FromCol(-1, -2, -3, -4));
        REQUIRE(m1 * m2 == SMatrix<float, 2, 2>::FromCol(22, 32, 11, 20));
        REQUIRE(MulEach(m1, m2) == SMatrix<float, 2, 2>::FromCol(4, 12, 24, 4));
        REQUIRE(DivEach(m1, m2) ==
                SMatrix<float, 2, 2>::FromCol(0.25f, 1.0 / 3.0f, 3 / 8.0f, 4));
    }
}