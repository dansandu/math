#include "dansandu/math/common.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::math::close;

TEST_CASE("common")
{
    REQUIRE(close(0.0001, 0.0005, 0.0, 1.0e-3));

    REQUIRE(!close(0.001, -0.0005, 0.0, 1.0e-3));
}
