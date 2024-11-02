#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;

TEST_CASE("matrix.order")
{
    SECTION("equality")
    {
        SECTION("static with static")
        {
            auto a = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

            auto b = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

            auto c = Matrix<int, 2, 3>{{{1, 2, 3}, {11, -1, 13}}};

            REQUIRE(a == b);

            REQUIRE(a != c);
        }

        SECTION("mixed static with dynamic")
        {
            auto a = Matrix<int, 1, dynamic>{{3, 5, 7}};

            auto b = Matrix<int, dynamic, 3>{{3, 5, 7}};

            auto c = Matrix<int, dynamic, 3>{{{3, 5, 7}, {3, 5, 7}}};

            REQUIRE(a == b);

            REQUIRE(a != c);
        }

        SECTION("dynamic with dynamic")
        {
            auto a = Matrix<int>{{120, 105, 130, 150}};

            auto b = Matrix<int>{{120, 105, 130, 150}};

            auto c = Matrix<int>{{{120, 105}, {130, 150}}};

            REQUIRE(a == b);

            REQUIRE(a != c);
        }
    }

    SECTION("strict order")
    {
        const auto a = Matrix<int>{{1, 2, 3}};

        const auto b = Matrix<int>{{5, 6, 7}};

        REQUIRE(a < b);

        REQUIRE(b > a);
    }

    SECTION("non-strict order")
    {
        const auto a = Matrix<int>{{1, 2, 3}};

        const auto b = Matrix<int>{{1, 2, 5}};

        REQUIRE(a <= b);

        REQUIRE(b >= a);
    }
}
