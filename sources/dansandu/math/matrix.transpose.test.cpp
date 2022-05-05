#include "dansandu/math/matrix.hpp"
#include "catchorg/catch/catch.hpp"

using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::transposed;

TEST_CASE("matrix.transpose")
{
    SECTION("row vector")
    {
        const auto matrix = Matrix<int, 1, 4>{{7, 11, 13, 17}};

        const auto expected = Matrix<int, 4, 1>{{7, 11, 13, 17}};

        const auto actual = transposed(matrix);

        REQUIRE(actual == expected);
    }

    SECTION("column vector")
    {
        const auto matrix = Matrix<int>{{1, 2, 3}};

        const auto expected = Matrix<int, 1, 3>{{1, 2, 3}};

        const auto actual = transposed(matrix);

        REQUIRE(actual == expected);
    }

    SECTION("matrix")
    {
        const auto matrix = Matrix<int>{{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

        const auto expected = Matrix<int, 3, 3>{{{1, 4, 7}, {2, 5, 8}, {3, 6, 9}}};

        const auto actual = transposed(matrix);

        REQUIRE(actual == expected);
    }
}
