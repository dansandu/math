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

    const auto& constantMatrix = matrix;

    SECTION("view")
    {
        const auto view = Slicer<1, 1, 1, 3>::slice(matrix);

        SECTION("data")
        {
            REQUIRE(view.rowCount() == 1);

            REQUIRE(view.columnCount() == 3);

            const auto expected = std::vector<int>{{6, 7, 8}};
            const auto actual = std::vector<int>{view.cbegin(), view.cend()};

            REQUIRE(actual == expected);
        }

        SECTION("subscript")
        {
            view.x() = 100;

            view(1) = 200;

            view(0, 2) = 300;

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 100, 200, 300, 9, 10, 11, 12}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            const auto expectedView = std::vector<int>{{100, 200, 300}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("implace addition")
        {
            const auto other = Matrix<int, 1, 3>{{100, 200, 300}};

            view += other;

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 106, 207, 308, 9, 10, 11, 12}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            const auto expectedView = std::vector<int>{{106, 207, 308}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("implace subtraction")
        {
            const auto other = Matrix<int, 1, 3>{{100, 200, 300}};

            view -= other;

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, -94, -193, -292, 9, 10, 11, 12}};
            const auto expectedView = std::vector<int>{{-94, -193, -292}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("additive inverse")
        {
            const auto other = -view;

            const auto expectedOther = std::vector<int>{{-6, -7, -8}};

            REQUIRE(std::vector<int>{other.cbegin(), other.cend()} == expectedOther);

            const auto expectedView = std::vector<int>{{6, 7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);

            const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};

            REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);
        }

        SECTION("scalar multiplication")
        {
            const auto result = 10 * view;

            REQUIRE(result.rowCount() == 1);

            REQUIRE(result.columnCount() == 3);

            const auto expectedResult = std::vector<int>{{60, 70, 80}};

            REQUIRE(std::vector<int>{result.cbegin(), result.cend()} == expectedResult);

            const auto expectedView = std::vector<int>{{6, 7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("scalar division")
        {
            const auto result = view / 2;

            REQUIRE(result.rowCount() == 1);

            REQUIRE(result.columnCount() == 3);

            const auto expectedResult = std::vector<int>{{3, 3, 4}};

            REQUIRE(std::vector<int>{result.cbegin(), result.cend()} == expectedResult);

            const auto expectedView = std::vector<int>{{6, 7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("addition")
        {
            const auto other = Matrix<int, 1, 3>{{10, 100, 1000}};

            const auto result = view + other;

            REQUIRE(result.rowCount() == 1);

            REQUIRE(result.columnCount() == 3);

            const auto expectedResult = std::vector<int>{{16, 107, 1008}};

            REQUIRE(std::vector<int>{result.cbegin(), result.cend()} == expectedResult);

            const auto expectedView = std::vector<int>{{6, 7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("subtraction")
        {
            const auto other = Slicer<2, 1, 1, 3>::slice(constantMatrix);

            const auto result = view - other;

            REQUIRE(result.rowCount() == 1);

            REQUIRE(result.columnCount() == 3);

            const auto expectedResult = std::vector<int>{{-4, -4, -4}};

            REQUIRE(std::vector<int>{result.cbegin(), result.cend()} == expectedResult);

            const auto expectedView = std::vector<int>{{6, 7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }

        SECTION("multiplication")
        {
            const auto other = Matrix<int, 3, 2>{{{10, 100}, {20, 200}, {30, 300}}};

            const auto result = view * other;

            REQUIRE(result.rowCount() == 1);

            REQUIRE(result.columnCount() == 2);

            const auto expectedResult = std::vector<int>{{440, 4400}};

            REQUIRE(std::vector<int>{result.cbegin(), result.cend()} == expectedResult);

            const auto expectedView = std::vector<int>{{6, 7, 8}};

            REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
        }
    }

    SECTION("view to view")
    {
        const auto view = Slicer<0, 1, 1, 2>::slice(Slicer<1, 1, 2, 3>::slice(matrix));

        REQUIRE(view.rowCount() == 1);

        REQUIRE(view.columnCount() == 2);

        const auto expected = std::vector<int>{{7, 8}};

        REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expected);

        view(0) = 0;

        const auto expectedMatrix = std::vector<int>{{1, 2, 3, 4, 5, 6, 0, 8, 9, 10, 11, 12}};

        REQUIRE(std::vector<int>{matrix.cbegin(), matrix.cend()} == expectedMatrix);

        const auto expectedView = std::vector<int>{{0, 8}};

        REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expectedView);
    }

    SECTION("container to constant view")
    {
        const auto view = Slicer<1, 2, 2, 1>::slice(constantMatrix);

        REQUIRE(view.rowCount() == 2);

        REQUIRE(view.columnCount() == 1);

        const auto expected = std::vector<int>{{7, 11}};

        REQUIRE(std::vector<int>{view.cbegin(), view.cend()} == expected);
    }
}
