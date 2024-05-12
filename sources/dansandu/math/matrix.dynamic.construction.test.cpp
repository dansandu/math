#include "dansandu/math/matrix.hpp"
#include "catchorg/catch/catch.hpp"

#include <vector>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;
using dansandu::math::matrix::Slicer;

TEST_CASE("matrix.dynamic.construction")
{
    SECTION("default")
    {
        const auto matrix = Matrix<int>{};

        REQUIRE(matrix.rowCount() == 0);

        REQUIRE(matrix.columnCount() == 0);

        const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
        const auto expected = std::vector<int>{};

        REQUIRE(actual == expected);
    }

    SECTION("array")
    {
        SECTION("vector")
        {
            const auto matrix = Matrix<int>{{1, 2, 3}};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 1);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3}};

            REQUIRE(actual == expected);
        }

        SECTION("matrix")
        {
            const auto matrix = Matrix<int>{{{1, 2}, {3, 4}, {5, 6}}};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 2);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("fill")
    {
        SECTION("vector")
        {
            const auto matrix = Matrix<int>{1, 3, 23};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{23, 23, 23}};

            REQUIRE(actual == expected);
        }

        SECTION("vector")
        {
            const auto matrix = Matrix<int>{3, 2, 7};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 2);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{7, 7, 7, 7, 7, 7}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("range")
    {
        SECTION("vector")
        {
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5}};

            const auto matrix = Matrix<int>{1, 5, expected.cbegin(), expected.cend()};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 5);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};

            REQUIRE(actual == expected);
        }

        SECTION("vector")
        {
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6, 7, 8}};

            const auto matrix = Matrix<int>{4, 2, expected.cbegin(), expected.cend()};

            REQUIRE(matrix.rowCount() == 4);

            REQUIRE(matrix.columnCount() == 2);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};

            REQUIRE(actual == expected);
        }
    }

    SECTION("copy")
    {
        const auto original = Matrix<int>{{{1, 2, 3}, {4, 5, 6}}};

        SECTION("container to container")
        {
            const auto matrix = Matrix<int>{original};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(actual == expected);
        }

        SECTION("container to view")
        {
            const auto matrix = ConstantMatrixView<int>{original};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(actual == expected);
        }

        SECTION("view to container")
        {
            const auto view = Slicer<1, 0>::slice(original, 1, 3);
            const auto matrix = Matrix<int>{view};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 3);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{4, 5, 6}};

            REQUIRE(actual == expected);
        }

        SECTION("view to view")
        {
            const auto view = Slicer<0, 1>::slice(original, 2, 1);
            const auto matrix = ConstantMatrixView<int>{view};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 1);

            const auto actual = std::vector<int>{matrix.cbegin(), matrix.cend()};
            const auto expected = std::vector<int>{{2, 5}};

            REQUIRE(actual == expected);
        }
    }

    SECTION("move")
    {
        auto original = Matrix<int>{{{1, 2, 3}, {4, 5, 6}}};

        SECTION("container to container")
        {
            const auto matrix = Matrix<int>{std::move(original)};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            REQUIRE(original.rowCount() == 0);

            REQUIRE(original.columnCount() == 0);

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 6}};
            const auto expectedOriginal = std::vector<int>{};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expectedOriginal);
        }

        SECTION("container to view")
        {
            const auto matrix = ConstantMatrixView<int>{std::move(original)};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 3);

            REQUIRE(original.rowCount() == 2);

            REQUIRE(original.columnCount() == 3);

            const auto expected = std::vector<int>{{1, 2, 3, 4, 5, 6}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expected);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expected);
        }

        const auto expectedOriginal = std::vector<int>{{1, 2, 3, 4, 5, 6}};

        SECTION("view to container")
        {
            const auto view = Slicer<0, 0>::slice(original, 2, 1);
            const auto matrix = Matrix<int>{std::move(view)};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 1);

            REQUIRE(original.rowCount() == 2);

            REQUIRE(original.columnCount() == 3);

            const auto expectedMatrix = std::vector<int>{{1, 4}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expectedOriginal);
        }

        SECTION("view to view")
        {
            const auto view = Slicer<1, 0>::slice(original, 1, 3);
            const auto matrix = MatrixView<int>{std::move(view)};

            REQUIRE(matrix.rowCount() == 1);

            REQUIRE(matrix.columnCount() == 3);

            REQUIRE(original.rowCount() == 2);

            REQUIRE(original.columnCount() == 3);

            const auto expectedMatrix = std::vector<int>{{4, 5, 6}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{original.cbegin(), original.cend()} == expectedOriginal);
        }
    }
}
