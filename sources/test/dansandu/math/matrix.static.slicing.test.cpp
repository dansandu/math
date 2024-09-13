#include "dansandu/math/matrix.hpp"
#include "catchorg/catch/catch.hpp"

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
            const auto expected = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expected);
        }

        SECTION("subscript")
        {
            view.x() = 100;

            view(1) = 200;

            view(0, 2) = 300;

            const auto expectedMatrix = Matrix<int, 3, 4>{{{1, 2, 3, 4}, {5, 100, 200, 300}, {9, 10, 11, 12}}};

            REQUIRE(matrix == expectedMatrix);

            const auto expectedView = Matrix<int, 1, 3>{{100, 200, 300}};

            REQUIRE(view == expectedView);
        }

        SECTION("implace addition")
        {
            const auto other = Matrix<int, 1, 3>{{100, 200, 300}};

            view += other;

            const auto expectedMatrix = Matrix<int, 3, 4>{{{1, 2, 3, 4}, {5, 106, 207, 308}, {9, 10, 11, 12}}};

            REQUIRE(matrix == expectedMatrix);

            const auto expectedView = Matrix<int, 1, 3>{{106, 207, 308}};

            REQUIRE(view == expectedView);
        }

        SECTION("implace subtraction")
        {
            const auto other = Matrix<int, 1, 3>{{100, 200, 300}};

            view -= other;

            const auto expectedMatrix = Matrix<int, 3, 4>{{{1, 2, 3, 4}, {5, -94, -193, -292}, {9, 10, 11, 12}}};

            REQUIRE(matrix == expectedMatrix);

            const auto expectedView = Matrix<int, 1, 3>{{-94, -193, -292}};

            REQUIRE(view == expectedView);
        }

        SECTION("additive inverse")
        {
            const auto other = -view;

            const auto expectedOther = Matrix<int, 1, 3>{{-6, -7, -8}};

            REQUIRE(other == expectedOther);

            const auto expectedView = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expectedView);

            const auto expectedMatrix = Matrix<int, 3, 4>{{{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}}};

            REQUIRE(matrix == expectedMatrix);
        }

        SECTION("scalar multiplication")
        {
            const auto result = 10 * view;

            const auto expectedResult = Matrix<int, 1, 3>{{60, 70, 80}};

            REQUIRE(result == expectedResult);

            const auto expectedView = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expectedView);
        }

        SECTION("scalar division")
        {
            const auto result = view / 2;

            const auto expectedResult = Matrix<int, 1, 3>{{3, 3, 4}};

            REQUIRE(result == expectedResult);

            const auto expectedView = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expectedView);
        }

        SECTION("addition")
        {
            const auto other = Matrix<int, 1, 3>{{10, 100, 1000}};

            const auto result = view + other;

            const auto expectedResult = Matrix<int, 1, 3>{{16, 107, 1008}};

            REQUIRE(result == expectedResult);

            const auto expectedView = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expectedView);
        }

        SECTION("subtraction")
        {
            const auto other = Slicer<2, 1, 1, 3>::slice(constantMatrix);

            const auto result = view - other;

            const auto expectedResult = Matrix<int, 1, 3>{{-4, -4, -4}};

            REQUIRE(result == expectedResult);

            const auto expectedView = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expectedView);
        }

        SECTION("multiplication")
        {
            const auto other = Matrix<int, 3, 2>{{{10, 100}, {20, 200}, {30, 300}}};

            const auto result = view * other;

            const auto expectedResult = Matrix<int, 1, 2>{{440, 4400}};

            REQUIRE(result == expectedResult);

            const auto expectedView = Matrix<int, 1, 3>{{6, 7, 8}};

            REQUIRE(view == expectedView);
        }
    }

    SECTION("view to view")
    {
        const auto view = Slicer<0, 2, 2, 1>::slice(Slicer<1, 1, 2, 3>::slice(matrix));

        const auto expectedView = Matrix<int, 2, 1>{{8, 12}};

        REQUIRE(view == expectedView);

        view(0) = 0;

        const auto expectedMatrix = Matrix<int, 3, 4>{{{1, 2, 3, 4}, {5, 6, 7, 0}, {9, 10, 11, 12}}};

        REQUIRE(matrix == expectedMatrix);

        const auto expectedMutatedView = Matrix<int, 2, 1>{{0, 12}};

        REQUIRE(view == expectedMutatedView);
    }

    SECTION("container to constant view")
    {
        const auto view = Slicer<1, 2, 2, 1>::slice(constantMatrix);

        const auto expected = Matrix<int, 2, 1>{{7, 11}};

        REQUIRE(view == expected);
    }
}
