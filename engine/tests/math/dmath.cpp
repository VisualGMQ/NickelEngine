#include "catch2/catch_test_macros.hpp"
#include "nickel/common/math/algorithm.hpp"
#include "nickel/common/math/matrix.hpp"

using namespace nickel;

TEST_CASE("matrix") {
    SECTION("initialize") {
        {
            Matrix<float> mat{2, 2};
            REQUIRE(mat[0][0] == 0);
            REQUIRE(mat[0][1] == 0);
            REQUIRE(mat[1][0] == 0);
            REQUIRE(mat[1][1] == 0);
        }
        {
            auto mat = Matrix<float>::FromCol(2, 2, 1, 2, 3, 4);
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 2);
            REQUIRE(mat[1][0] == 3);
            REQUIRE(mat[1][1] == 4);
        }
        {
            auto mat = Matrix<float>::FromRow(2, 2, 1, 2, 3, 4);
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 3);
            REQUIRE(mat[1][0] == 2);
            REQUIRE(mat[1][1] == 4);
        }
        {
            auto mat = Matrix<float>::Ones(2, 2);
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 1);
            REQUIRE(mat[1][0] == 1);
            REQUIRE(mat[1][1] == 1);
        }
        {
            auto mat = Matrix<float>::Identity(2, 2);
            REQUIRE(mat[0][0] == 1);
            REQUIRE(mat[0][1] == 0);
            REQUIRE(mat[1][0] == 0);
            REQUIRE(mat[1][1] == 1);
        }
        {
            auto mat = Matrix<float>::FillWith(2, 2, 8);
            REQUIRE(mat[0][0] == 8);
            REQUIRE(mat[0][1] == 8);
            REQUIRE(mat[1][0] == 8);
            REQUIRE(mat[1][1] == 8);
        }
    }

    SECTION("compare") {
        auto m = Matrix<float>::FromCol(2, 2, 1, 2, 3, 4);
        REQUIRE(m == Matrix<float>::FromCol(2, 2, 1, 2, 3, 4));
        REQUIRE(m != Matrix<float>::FromRow(2, 2, 1, 2, 3, 4));
        REQUIRE(m != Matrix<float>::FromCol(3, 2, 1, 2, 3, 4));
    }
    
    SECTION("operations") {
        auto m1 = Matrix<float>::FromCol(2, 2, 1, 2, 3, 4);
        auto m2 = Matrix<float>::FromCol(2, 2, 4, 6, 8, 1);
        REQUIRE(m1 + m2 == Matrix<float>::FromCol(2, 2, 5, 8, 11, 5));
        REQUIRE(m1 - m2 == Matrix<float>::FromCol(2, 2, -3, -4, -5, 3));
        REQUIRE(-m1 == Matrix<float>::FromCol(2, 2, -1, -2, -3, -4));
        REQUIRE(m1 * m2 == Matrix<float>::FromCol(2, 2, 22, 32, 11, 20));
    }
}
