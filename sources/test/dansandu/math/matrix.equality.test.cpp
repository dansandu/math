#include "dansandu/math/matrix.hpp"
#include "catchorg/catch/catch.hpp"

using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;

TEST_CASE("matrix.equality")
{
    SECTION("static")
    {
        const auto matrix = Matrix<int, 3, 2>{{{11, 13}, {17, 19}, {23, 29}}};

        SECTION("to static match")
        {
            const auto other = Matrix<int, 3, 2>{{{11, 13}, {17, 19}, {23, 29}}};

            REQUIRE(matrix == other);
        }

        SECTION("to dynamic rows match")
        {
            const auto other = Matrix<int, dynamic, 2>{{{11, 13}, {17, 19}, {23, 29}}};

            REQUIRE(matrix == other);
        }

        SECTION("to dynamic columns match")
        {
            const auto other = Matrix<int, 3, dynamic>{{{11, 13}, {17, 19}, {23, 29}}};

            REQUIRE(matrix == other);
        }

        SECTION("to dynamic rows and columns match")
        {
            const auto other = Matrix<int>{{{11, 13}, {17, 19}, {23, 29}}};

            REQUIRE(matrix == other);
        }

        SECTION("to dynamic rows mismatch")
        {
            const auto other = Matrix<int, dynamic, 2>{{{11, 13}, {17, 19}}};

            REQUIRE(matrix != other);
        }

        SECTION("to dynamic columns mismatch")
        {
            const auto other = Matrix<int, 3, dynamic>{{11, 13, 17}};

            REQUIRE(matrix != other);
        }

        SECTION("to dynamic rows and columns mismatch")
        {
            const auto other = Matrix<int>{{{11, 13}, {17, 19}}};

            REQUIRE(matrix != other);
        }
    }

    SECTION("dynamic")
    {
        const auto matrix = Matrix<int>{{{31, 37, 41}, {43, 47, 53}}};

        SECTION("to dynamic rows match")
        {
            const auto other = Matrix<int, dynamic, 3>{{{31, 37, 41}, {43, 47, 53}}};

            REQUIRE(matrix == other);
        }

        SECTION("to dynamic columns match")
        {
            const auto other = Matrix<int, 2, dynamic>{{{31, 37, 41}, {43, 47, 53}}};

            REQUIRE(matrix == other);
        }

        SECTION("to dynamic rows and columns match")
        {
            const auto other = Matrix<int>{{{31, 37, 41}, {43, 47, 53}}};

            REQUIRE(matrix == other);
        }
    }
}
