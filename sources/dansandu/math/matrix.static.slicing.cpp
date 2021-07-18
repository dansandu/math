#include "catchorg/catch/catch.hpp"
#include "dansandu/math/matrix.hpp"

#include <vector>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;
using dansandu::math::matrix::Slicer;

TEST_CASE("matrix.static.slicing")
{
    auto matrix = Matrix<int, 3, 4>{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}};

    SECTION("view")
    {
        const auto view = Slicer<1, 1, 2, 3>::slice(matrix);

        REQUIRE(view.rowCount() == 2);

        REQUIRE(view.columnCount() == 3);

        SECTION("unmutated")
        {
            const auto expected = std::vector<int>{{6, 7, 8, 10, 11, 12}};
            const auto actual = std::vector<int>{view.cbegin(), view.cend()};

            REQUIRE(actual == expected);
        }

        SECTION("mutated")
        {
            view(0, 2) = 0;

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 6, 7, 0, 9, 10, 11, 12}};
            const auto expectedView = std::vector<int>{{6, 7, 0, 10, 11, 12}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }
    }

    SECTION("view to view")
    {
        const auto view = Slicer<0, 1, 1, 2>::slice(Slicer<1, 1, 2, 3>::slice(matrix));

        REQUIRE(view.rowCount() == 1);

        REQUIRE(view.columnCount() == 2);

        SECTION("unmutated")
        {
            const auto expected = std::vector<int>{{7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expected);
        }

        SECTION("mutated")
        {
            view(0, 0) = 0;

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 6, 0, 8, 9, 10, 11, 12}};
            const auto expectedView = std::vector<int>{{0, 8}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }
    }

    const auto& constantMatrix = matrix;

    SECTION("container to constant view")
    {
        const auto view = Slicer<1, 2, 2, 1>::slice(constantMatrix);

        REQUIRE(view.rowCount() == 2);

        REQUIRE(view.columnCount() == 1);

        const auto expected = std::vector<int>{{7, 11}};

        REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expected);
    }
}
