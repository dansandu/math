#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <type_traits>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;

TEST_CASE("matrix.dynamic.assignment")
{
    SECTION("container")
    {
        REQUIRE(std::is_nothrow_move_assignable_v<Matrix<int>>);

        const auto assigned = Matrix<int>{{{1, 2, 3}, {4, 5, 6}}};

        auto assignee = Matrix<int>{{{10, 11}, {12, 13}, {14, 15}}};

        assignee = assigned;

        REQUIRE(assignee.rowCount() == 2);

        REQUIRE(assignee.columnCount() == 3);

        REQUIRE(assignee == assigned);
    }

    SECTION("constant view")
    {
        REQUIRE(std::is_nothrow_move_assignable_v<ConstantMatrixView<int>>);

        const auto a = Matrix<int>{{{1, 2, 3}, {4, 5, 6}}};

        const auto b = Matrix<int>{{{1, 2}, {3, 4}}};

        ConstantMatrixView<int> view = a;

        view = b;

        REQUIRE(view.rowCount() == 2);

        REQUIRE(view.columnCount() == 2);

        REQUIRE(view == b);
    }

    SECTION("view")
    {
        REQUIRE(std::is_nothrow_move_assignable_v<MatrixView<int>>);

        auto a = Matrix<int>{{{1, 2}, {3, 4}, {5, 6}}};

        auto b = Matrix<int>{{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}};

        MatrixView<int> view = a;

        view = b;

        REQUIRE(view.rowCount() == 3);

        REQUIRE(view.columnCount() == 3);

        REQUIRE(view == b);
    }
}
