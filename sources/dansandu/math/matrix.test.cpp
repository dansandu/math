#include "catchorg/catch/catch.hpp"
#include "dansandu/math/matrix.hpp"

#include <stdexcept>

using Catch::Detail::Approx;
using dansandu::math::matrix::close;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::magnitude;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::normalized;

TEST_CASE("dansandu::math::matrix::Matrix") {
    SECTION("default constructor") {
        auto vector = Matrix<int, 3, 1>{};

        REQUIRE(vector.rowCount() == 3);

        REQUIRE(vector.columnCount() == 1);

        REQUIRE(vector.length() == 3);

        REQUIRE(vector(0, 0) == 0);

        REQUIRE(vector(1, 0) == 0);

        REQUIRE(vector(2, 0) == 0);
    }

    SECTION("1D array constructor") {
        auto vector = Matrix<int, 1, 3>{7, 11, 13};

        REQUIRE(vector.rowCount() == 1);

        REQUIRE(vector.columnCount() == 3);

        REQUIRE(vector.length() == 3);

        REQUIRE(vector(0, 0) == 7);

        REQUIRE(vector(0, 1) == 11);

        REQUIRE(vector(0, 2) == 13);
    }

    SECTION("2D array constructor") {
        auto matrix = Matrix<int, 2, 3>{{{17, 19, 23}, {29, 31, 37}}};

        REQUIRE(matrix.rowCount() == 2);

        REQUIRE(matrix.columnCount() == 3);

        REQUIRE(matrix(0, 0) == 17);

        REQUIRE(matrix(0, 1) == 19);

        REQUIRE(matrix(0, 2) == 23);

        REQUIRE(matrix(1, 0) == 29);

        REQUIRE(matrix(1, 1) == 31);

        REQUIRE(matrix(1, 2) == 37);
    }

    SECTION("fill constructor") {
        auto matrix = Matrix<int, 3, 2>{-7};
        auto expected = {-7, -7, -7, -7, -7, -7};

        REQUIRE(std::equal(matrix.cbegin(), matrix.cend(), expected.begin(), expected.end()));
    }

    SECTION("components constructor") {
        auto x = 3;
        auto y = 5;
        auto z = 7;
        auto matrix = Matrix<int, 3, 1>{x, y, z};

        REQUIRE(matrix.x() == x);

        REQUIRE(matrix.y() == y);

        REQUIRE(matrix.z() == z);
    }

    SECTION("bad indexing throws") {
        auto vector = Matrix<int, 3, 1>{};

        REQUIRE_THROWS_AS(vector(3, 1), std::out_of_range);

        REQUIRE_THROWS_AS(vector(-1, 0), std::out_of_range);

        REQUIRE_THROWS_AS(vector(0, -1), std::out_of_range);

        REQUIRE_THROWS_AS(vector(2, 2), std::out_of_range);

        REQUIRE_THROWS_AS(vector(4, 0), std::out_of_range);
    }

    SECTION("vector subscript") {
        auto vector = Matrix<int, 3, 1>{1, 3, 5};

        REQUIRE(vector.x() == 1);

        REQUIRE(vector.y() == 3);

        REQUIRE(vector.z() == 5);

        REQUIRE(vector(0) == 1);

        REQUIRE(vector(1) == 3);

        REQUIRE(vector(2) == 5);
    }

    SECTION("binary matrix-matrix operations") {
        auto a = Matrix<int, 3, 1>{10, 15, 20};
        auto b = Matrix<int, 3, 1>{1, 3, 5};

        SECTION("addition") { REQUIRE(a + b == Matrix<int, 3, 1>{11, 18, 25}); }

        SECTION("subtraction") { REQUIRE(a - b == Matrix<int, 3, 1>{9, 12, 15}); }

        SECTION("dot product") { REQUIRE(dotProduct(a, b) == 155); }

        SECTION("cross product") { REQUIRE(crossProduct(a, b) == Matrix<int, 3, 1>{15, -30, 15}); }
    }

    SECTION("matrix multiplication") {
        auto a = Matrix<int, 1, 3>{1, 5, 7};
        auto b = Matrix<int, 3, 2>{{{11, 13}, {17, 19}, {23, 29}}};

        REQUIRE(a * b == Matrix<int, 1, 2>{257, 311});
    }

    SECTION("binary scalar-matrix operations") {
        auto matrix = Matrix<int, 2, 2>{{{10, 15}, {20, 25}}};

        SECTION("multiplication") { REQUIRE(2 * matrix == Matrix<int, 2, 2>{{{20, 30}, {40, 50}}}); }

        SECTION("division") { REQUIRE(matrix / 5 == Matrix<int, 2, 2>{{{2, 3}, {4, 5}}}); }
    }

    SECTION("magnitude") {
        auto vector = Matrix<double, 3, 1>{3.0, 4.0, 5.0};

        REQUIRE(magnitude(vector) == Approx(7.071067f));
    }

    SECTION("norm") {
        constexpr auto precision = 1e-5f;
        auto vector = Matrix<double, 3, 1>{3.0, 4.0, 5.0};
        auto expected = Matrix<double, 3, 1>{0.424264, 0.565685, 0.707106};

        REQUIRE(close(normalized(vector), expected, precision));
    }
}
