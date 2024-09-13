#include "dansandu/math/matrix.hpp"
#include "catchorg/catch/catch.hpp"

using dansandu::math::matrix::identity;
using dansandu::math::matrix::Matrix;

TEST_CASE("matrix.identity")
{
    REQUIRE(identity<int>(3) == Matrix<int>{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}});

    REQUIRE(identity<int>(2, 1) == Matrix<int>{{1, 0}});

    REQUIRE(identity<int, 2, 2>() == Matrix<int>{{{1, 0}, {0, 1}}});

    REQUIRE(identity<int, 3>() == Matrix<int>{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}});
}
