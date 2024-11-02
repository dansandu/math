#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::math::matrix::abs;
using dansandu::math::matrix::Matrix;

TEST_CASE("matrix.abs")
{
    const auto matrix = Matrix<int>{{{-1, 2, 3}, {4, -5, 6}, {-7, -8, -9}}};

    const auto expected = Matrix<int>{{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

    REQUIRE(abs(matrix) == expected);
}
