#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "common/math/matrix.hpp"
#include "common/math/smatrix.hpp"
#include "common/math/math.hpp"

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

TEST_CASE("math") {
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
