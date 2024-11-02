#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;

TEST_CASE("matrix.addition")
{
    SECTION("static with static")
    {
        SECTION("dimensions match")
        {
            auto a = Matrix<int, 2, 2>{{{-59, 35}, {3, 0}}};

            const auto b = Matrix<int, 2, 2>{{{9, 34}, {29, 2}}};

            const auto expected = Matrix<int, 2, 2>{{{-50, 69}, {32, 2}}};

            REQUIRE(a + b == expected);

            SECTION("view")
            {
                const auto av = MatrixView<int, 2, 2>{a};

                av += b;

                REQUIRE(av == a);

                REQUIRE(av == expected);
            }
        }
    }

    SECTION("static with dynamic")
    {
        SECTION("dimensions match")
        {
            const auto a = Matrix<int, 3, 1>{{10, 15, 20}};

            const auto b = Matrix<int>{{120, 105, 130}};

            const auto expected = Matrix<int, 3, 1>{{130, 120, 150}};

            REQUIRE(a + b == expected);
        }

        SECTION("dimensions mismatch")
        {
            const auto a = Matrix<int, 3, 1>{{10, 15, 20}};
            const auto b = Matrix<int>{{120, 105}};

            REQUIRE_THROW(a + b, std::logic_error);
        }
    }

    SECTION("mixed static with dynamic")
    {
        SECTION("dimensions match")
        {
            const auto a = Matrix<int, 1, dynamic>{{3, 5, 7}};

            const auto b = Matrix<int, dynamic, 3>{{10, 100, 1000}};

            const auto expected = Matrix<int, 1, dynamic>{{13, 105, 1007}};

            REQUIRE(a + b == expected);
        }

        SECTION("dimensions mismatch")
        {
            const auto a = Matrix<int, 1, dynamic>{{3, 5}};

            const auto b = Matrix<int, dynamic, 3>{{10, 100, 1000}};

            REQUIRE_THROW(a + b, std::logic_error);
        }
    }

    SECTION("dynamic with dynamic")
    {
        SECTION("dimensions match")
        {
            const auto a = Matrix<int>{{1, 2}};

            const auto b = Matrix<int>{{30, 40}};

            const auto expected = Matrix<int>{{31, 42}};

            REQUIRE(a + b == expected);
        }

        SECTION("dimensions mismatch")
        {
            const auto a = Matrix<int>{{1, 2}};

            const auto b = Matrix<int>{{30, 40, 50}};

            REQUIRE_THROW(a + b, std::logic_error);
        }
    }

    SECTION("matrix with scalar")
    {
        const auto matrix = Matrix<int>{{{1, 2, 3}, {4, 5, 6}}};

        const auto scalar = 10;

        const auto expected = Matrix<int>{{{11, 12, 13}, {14, 15, 16}}};

        REQUIRE(matrix + scalar == expected);

        REQUIRE(scalar + matrix == expected);
    }
}
