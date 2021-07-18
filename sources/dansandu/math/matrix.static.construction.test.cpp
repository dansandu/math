#include "catchorg/catch/catch.hpp"
#include "dansandu/math/matrix.hpp"

#include <vector>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;
using dansandu::math::matrix::Slicer;

TEST_CASE("matrix.static.construction")
{
    SECTION("default")
    {
        SECTION("null")
        {
            const auto matrix = Matrix<int, 0, 0>{};

            REQUIRE(matrix.rowCount() == 0);

            REQUIRE(matrix.columnCount() == 0);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{};

            REQUIRE(actual == expected);
        }

        SECTION("vector")
        {
            const auto matrix = Matrix<int, 3, 1>{};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 1);

            REQUIRE(matrix.length() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{0, 0, 0}};

            REQUIRE(actual == expected);
        }

        SECTION("matrix")
        {
            const auto matrix = Matrix<int, 2, 3>{};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{0, 0, 0, 0, 0, 0}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("array")
    {
        SECTION("vector")
        {
            const auto matrix = Matrix<int, 4, 1>{{11, 13, 17, 23}};

            REQUIRE(matrix.rowCount() == 4);

            REQUIRE(matrix.columnCount() == 1);

            REQUIRE(matrix.length() == 4);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{11, 13, 17, 23}};

            REQUIRE(actual == expected);
        }

        SECTION("matrix")
        {
            const auto matrix = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3, 11, 12, 13}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("fill")
    {
        SECTION("vector")
        {
            const auto matrix = Matrix<int, 1, 3>{1, 3, 9};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{9, 9, 9}};

            REQUIRE(actual == expected);
        }

        SECTION("matrix")
        {
            const auto matrix = Matrix<int, 3, 2>{3, 2, 17};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 2);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{17, 17, 17, 17, 17, 17}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("range")
    {
        SECTION("vector")
        {
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6, 7}};

            const auto matrix = Matrix<int, 7, 1>{7, 1, expected.cbegin(), expected.cend()};

            REQUIRE(matrix.rowCount() == 7);

            REQUIRE(matrix.columnCount() == 1);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};

            REQUIRE(actual == expected);
        }

        SECTION("vector")
        {
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6, 7, 8}};

            const auto matrix = Matrix<int, 2, 4>{2, 4, expected.cbegin(), expected.cend()};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 4);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};

            REQUIRE(actual == expected);
        }
    }

    SECTION("copy")
    {
        const auto original = Matrix<int, 2, 3>{{{1, 2, 3}, {4, 5, 6}}};

        SECTION("container to container")
        {
            const auto matrix = Matrix<int, 2, 3>{original};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(actual == expected);
        }

        SECTION("container to view")
        {
            const auto matrix = ConstantMatrixView<int, 2, 3>{original};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(actual == expected);
        }

        SECTION("view to container")
        {
            const auto view = Slicer<1, 1, 1, 2>::slice(original);
            const auto matrix = Matrix<int, 1, 2>{view};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 2);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{5, 6}};

            REQUIRE(actual == expected);
        }

        SECTION("view to view")
        {
            const auto view = Slicer<1, 1, 1, 2>::slice(original);
            const auto matrix = ConstantMatrixView<int, 1, 2>{view};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 2);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{5, 6}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("move")
    {
        auto original = Matrix<int, 3, 2>{{{1, 2}, {3, 4}, {5, 6}}};

        SECTION("container to container")
        {
            const auto matrix = Matrix<int, 3, 2>{std::move(original)};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 2);

            REQUIRE(original.rowCount() == 3);

            REQUIRE(original.columnCount() == 2);

            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expected);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expected);
        }

        SECTION("container to view")
        {
            const auto matrix = ConstantMatrixView<int, 3, 2>{std::move(original)};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 2);

            REQUIRE(original.rowCount() == 3);

            REQUIRE(original.columnCount() == 2);

            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expected);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expected);
        }

        const auto expectedOriginal = std::vector<int>{{1, 2, 3, 4, 5, 6}};

        SECTION("view to container")
        {
            const auto view = Slicer<2, 0, 1, 2>::slice(original);
            const auto matrix = Matrix<int, 1, 2>{std::move(view)};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 2);

            REQUIRE(original.rowCount() == 3);

            REQUIRE(original.columnCount() == 2);

            const auto expectedMatrix = std::vector<int>{{5, 6}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expectedOriginal);
        }

        SECTION("view to view")
        {
            const auto view = Slicer<1, 0, 2, 1>::slice(original);
            const auto matrix = ConstantMatrixView<int, 2, 1>{std::move(view)};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 1);

            REQUIRE(original.rowCount() == 3);

            REQUIRE(original.columnCount() == 2);

            const auto expectedMatrix = std::vector<int>{{3, 5}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expectedOriginal);
        }
    }
}
