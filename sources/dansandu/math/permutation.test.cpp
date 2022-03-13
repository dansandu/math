#include "dansandu/math/permutation.hpp"
#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/exception.hpp"

#include <vector>

using dansandu::math::permutation::getIdentityPermutation;
using dansandu::math::permutation::getInvertedPermutation;

TEST_CASE("permutation")
{
    SECTION("identity")
    {
        SECTION("non-zero length")
        {
            const auto actual = getIdentityPermutation(5);
            const auto expected = std::vector<int>{{0, 1, 2, 3, 4}};

            REQUIRE(actual == expected);
        }

        SECTION("zero length")
        {
            const auto actual = getIdentityPermutation(0);
            const auto expected = std::vector<int>{};

            REQUIRE(actual == expected);
        }

        SECTION("negative length")
        {
            REQUIRE_THROWS_AS(getIdentityPermutation(-2), std::logic_error);
        }
    }

    SECTION("inversion")
    {
        const auto actual = getInvertedPermutation({4, 0, 2, 5, 1, 3});
        const auto expected = std::vector<int>{{1, 4, 2, 5, 0, 3}};

        REQUIRE(actual == expected);
    }
}
