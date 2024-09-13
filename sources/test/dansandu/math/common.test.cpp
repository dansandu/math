#include "dansandu/math/common.hpp"
#include "catchorg/catch/catch.hpp"

using dansandu::math::common::close;

TEST_CASE("common")
{
    REQUIRE(close(0.0001, 0.0005, 1.0e-3));

    REQUIRE(!close(0.001, -0.0005, 1.0e-3));
}
