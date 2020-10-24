#include "catchorg/catch/catch.hpp"
#include "dansandu/math/matrix.hpp"

#include <stdexcept>

using Catch::Detail::Approx;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;

TEST_CASE("dansandu::math::matrix::Matrix")
{
    SECTION("default constructor")
    {
        SECTION("static x static matrix")
        {
            SECTION("null matrix")
            {
                auto matrix = Matrix<int, 0, 0>{};

                REQUIRE(matrix.rowCount() == 0);

                REQUIRE(matrix.columnCount() == 0);
            }

            SECTION("row vector")
            {
                auto vector = Matrix<int, 1, 2>{};

                REQUIRE(vector.rowCount() == 1);

                REQUIRE(vector.columnCount() == 2);

                REQUIRE(vector.length() == 2);

                REQUIRE(vector(0, 0) == 0);

                REQUIRE(vector(0, 1) == 0);

                REQUIRE_THROWS_AS(vector(2), std::out_of_range);

                REQUIRE_THROWS_AS(vector(0, 2), std::out_of_range);

                REQUIRE_THROWS_AS(vector(1, 0), std::out_of_range);
            }

            SECTION("column vector")
            {
                auto vector = Matrix<int, 3, 1>{};

                REQUIRE(vector.rowCount() == 3);

                REQUIRE(vector.columnCount() == 1);

                REQUIRE(vector.length() == 3);

                REQUIRE(vector(0, 0) == 0);

                REQUIRE(vector(1, 0) == 0);

                REQUIRE(vector(2, 0) == 0);

                REQUIRE_THROWS_AS(vector(3), std::out_of_range);

                REQUIRE_THROWS_AS(vector(0, 1), std::out_of_range);

                REQUIRE_THROWS_AS(vector(3, 0), std::out_of_range);
            }

            SECTION("non-vector")
            {
                auto matrix = Matrix<int, 2, 3>{};

                REQUIRE(matrix.rowCount() == 2);

                REQUIRE(matrix.columnCount() == 3);

                REQUIRE(matrix(0, 0) == 0);

                REQUIRE(matrix(0, 1) == 0);

                REQUIRE(matrix(0, 2) == 0);

                REQUIRE(matrix(1, 0) == 0);

                REQUIRE(matrix(1, 1) == 0);

                REQUIRE(matrix(1, 2) == 0);

                REQUIRE_THROWS_AS(matrix(0, 3), std::out_of_range);

                REQUIRE_THROWS_AS(matrix(3, 0), std::out_of_range);
            }
        }

        SECTION("null dynamic x dynamic matrix")
        {
            auto matrix = Matrix<int, dynamic, dynamic>{};

            REQUIRE(matrix.rowCount() == 0);

            REQUIRE(matrix.columnCount() == 0);
        }

        SECTION("null static x dynamic matrix")
        {
            auto matrix = Matrix<int, 2, dynamic>{};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 0);

            REQUIRE_THROWS_AS(matrix.length(), std::runtime_error);

            REQUIRE_THROWS_AS(matrix(0), std::out_of_range);

            REQUIRE_THROWS_AS(matrix(0, 0), std::out_of_range);

            REQUIRE_THROWS_AS(matrix.x(), std::out_of_range);
        }

        SECTION("null dynamic x static matrix")
        {
            auto matrix = Matrix<int, dynamic, 1>{};

            REQUIRE(matrix.rowCount() == 0);

            REQUIRE(matrix.columnCount() == 1);

            REQUIRE(matrix.length() == 0);

            REQUIRE_THROWS_AS(matrix(0), std::out_of_range);

            REQUIRE_THROWS_AS(matrix(0, 0), std::out_of_range);

            REQUIRE_THROWS_AS(matrix.x(), std::out_of_range);
        }
    }

    SECTION("vector initialization")
    {
        SECTION("static vector")
        {
            auto vector = Matrix<int, 4, 1>{{11, 13, 17, 23}};

            REQUIRE(vector.rowCount() == 4);

            REQUIRE(vector.columnCount() == 1);

            REQUIRE(vector.length() == 4);

            REQUIRE(vector(0) == 11);

            REQUIRE(vector(1) == 13);

            REQUIRE(vector(2) == 17);

            REQUIRE(vector(3) == 23);

            REQUIRE(vector.x() == 11);

            REQUIRE(vector.y() == 13);

            REQUIRE(vector.z() == 17);

            REQUIRE(vector.w() == 23);

            REQUIRE(vector(0, 0) == 11);

            REQUIRE(vector(1, 0) == 13);

            REQUIRE(vector(2, 0) == 17);

            REQUIRE(vector(3, 0) == 23);

            auto sameVector = Matrix<int, 4, 1>{{11, 13, 17, 23}};

            REQUIRE(vector == sameVector);

            auto otherVector = Matrix<int, 4, 1>{{11, 13, 17, 20}};

            REQUIRE(vector != otherVector);
        }
    }

    SECTION("matrix initialization")
    {
        auto matrix = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

        REQUIRE(matrix.rowCount() == 2);

        REQUIRE(matrix.columnCount() == 3);

        REQUIRE(matrix(0, 0) == 1);

        REQUIRE(matrix(0, 1) == 2);

        REQUIRE(matrix(0, 2) == 3);

        REQUIRE(matrix(1, 0) == 11);

        REQUIRE(matrix(1, 1) == 12);

        REQUIRE(matrix(1, 2) == 13);

        auto sameMatrix = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

        REQUIRE(matrix == sameMatrix);

        auto otherMatrix = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 22, 13}}};

        REQUIRE(matrix != otherMatrix);
    }
}
