#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <type_traits>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;

TEST_CASE("matrix.static.assignment")
{
    SECTION("container")
    {
        REQUIRE(std::is_nothrow_move_assignable_v<Matrix<int, 2, 3>>);

        const auto assigned = Matrix<int, 2, 3>{{{1, 2, 3}, {4, 5, 6}}};

        auto assignee = Matrix<int, 2, 3>{{{10, 20, 30}, {40, 50, 60}}};

        assignee = assigned;

        REQUIRE(assignee.rowCount() == 2);

        REQUIRE(assignee.columnCount() == 3);

        REQUIRE(assignee == assigned);
    }

    SECTION("constant view")
    {
        REQUIRE(std::is_nothrow_move_assignable_v<ConstantMatrixView<int, 2, 3>>);

        const auto a = Matrix<int, 2, 3>{{{1, 2, 3}, {4, 5, 6}}};

        const auto b = Matrix<int, 2, 3>{{{10, 20, 30}, {40, 50, 60}}};

        ConstantMatrixView<int, 2, 3> view = a;

        view = b;

        REQUIRE(view.rowCount() == 2);

        REQUIRE(view.columnCount() == 3);

        REQUIRE(view == b);
    }

    SECTION("view")
    {
        REQUIRE(std::is_nothrow_move_assignable_v<MatrixView<int>>);

        auto a = Matrix<int, 2, 3>{{{1, 2, 3}, {4, 5, 6}}};

        auto b = Matrix<int, 2, 3>{{{10, 20, 30}, {40, 50, 60}}};

        MatrixView<int, 2, 3> view = a;

        view = b;

        REQUIRE(view.rowCount() == 2);

        REQUIRE(view.columnCount() == 3);

        REQUIRE(view == b);
    }
}
