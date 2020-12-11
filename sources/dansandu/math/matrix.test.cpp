#include "catchorg/catch/catch.hpp"
#include "dansandu/math/matrix.hpp"

#include <stdexcept>

using Catch::Detail::Approx;
using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::magnitude;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::Slicer;

TEST_CASE("Matrix")
{
    SECTION("default constructor")
    {
        SECTION("static x static matrix")
        {
            SECTION("null")
            {
                const auto matrix = Matrix<int, 0, 0>{};

                REQUIRE(matrix.rowCount() == 0);

                REQUIRE(matrix.columnCount() == 0);
            }

            SECTION("row vector")
            {
                const auto vector = Matrix<int, 1, 2>{};

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
                const auto vector = Matrix<int, 3, 1>{};

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
                const auto matrix = Matrix<int, 2, 3>{};

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

        SECTION("null dynamic x dynamic")
        {
            const auto matrix = Matrix<int, dynamic, dynamic>{};

            REQUIRE(matrix.rowCount() == 0);

            REQUIRE(matrix.columnCount() == 0);
        }

        SECTION("null static x dynamic")
        {
            const auto matrix = Matrix<int, 2, dynamic>{};

            REQUIRE(matrix.rowCount() == 2);

            REQUIRE(matrix.columnCount() == 0);

            REQUIRE_THROWS_AS(matrix.length(), std::logic_error);

            REQUIRE_THROWS_AS(matrix(0), std::out_of_range);

            REQUIRE_THROWS_AS(matrix(0, 0), std::out_of_range);

            REQUIRE_THROWS_AS(matrix.x(), std::out_of_range);
        }

        SECTION("null dynamic x static")
        {
            const auto matrix = Matrix<int, dynamic, 1>{};

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
        SECTION("static")
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
        }
    }

    SECTION("matrix initialization")
    {
        SECTION("static")
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
        }

        SECTION("dynamic")
        {
            auto matrix = Matrix<int, dynamic, dynamic>{{{1, 2}, {11, 12}, {21, 22}}};

            REQUIRE(matrix.rowCount() == 3);

            REQUIRE(matrix.columnCount() == 2);

            REQUIRE(matrix(0, 0) == 1);

            REQUIRE(matrix(0, 1) == 2);

            REQUIRE(matrix(1, 0) == 11);

            REQUIRE(matrix(1, 1) == 12);

            REQUIRE(matrix(2, 0) == 21);

            REQUIRE(matrix(2, 1) == 22);
        }
    }

    SECTION("fill constructor")
    {
        SECTION("valid dimensions")
        {
            auto matrix = Matrix<int, dynamic, dynamic>{5, 7, 9};

            REQUIRE(matrix.rowCount() == 5);

            REQUIRE(matrix.columnCount() == 7);

            REQUIRE(std::all_of(matrix.cbegin(), matrix.cend(), [](auto e) { return e == 9; }));
        }

        SECTION("invalid dimensions")
        {
            REQUIRE_THROWS_AS(Matrix<double>(-3, 5), std::out_of_range);

            REQUIRE_THROWS_AS(Matrix<double>(5, -1), std::out_of_range);
        }
    }

    SECTION("equality")
    {
        SECTION("static with static")
        {
            auto a = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

            auto b = Matrix<int, 2, 3>{{{1, 2, 3}, {11, 12, 13}}};

            auto c = Matrix<int, 2, 3>{{{1, 2, 3}, {11, -1, 13}}};

            REQUIRE(a == b);

            REQUIRE(a != c);
        }

        SECTION("mixed static with dynamic")
        {
            auto a = Matrix<int, 1, dynamic>{{3, 5, 7}};

            auto b = Matrix<int, dynamic, 3>{{3, 5, 7}};

            auto c = Matrix<int, dynamic, 3>{{{3, 5, 7}, {3, 5, 7}}};

            REQUIRE(a == b);

            REQUIRE(a != c);
        }

        SECTION("dynamic with dynamic")
        {
            auto a = Matrix<int, dynamic, dynamic>{{120, 105, 130, 150}};

            auto b = Matrix<int, dynamic, dynamic>{{120, 105, 130, 150}};

            auto c = Matrix<int, dynamic, dynamic>{{{120, 105}, {130, 150}}};

            REQUIRE(a == b);

            REQUIRE(a != c);
        }
    }

    SECTION("addition")
    {
        SECTION("static with static")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 2, 2>{{{-59, 35}, {3, 0}}};
                auto b = Matrix<int, 2, 2>{{{9, 34}, {29, 2}}};
                auto expected = Matrix<int, 2, 2>{{{-50, 69}, {32, 2}}};

                REQUIRE(a + b == expected);

                SECTION("view")
                {
                    auto av = MatrixView<int, 2, 2>{a};

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
                auto a = Matrix<int, 3, 1>{{10, 15, 20}};
                auto b = Matrix<int, dynamic, dynamic>{{120, 105, 130}};
                auto expected = Matrix<int, 3, 1>{{130, 120, 150}};

                REQUIRE(a + b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, 3, 1>{{10, 15, 20}};
                auto b = Matrix<int, dynamic, dynamic>{{120, 105}};

                REQUIRE_THROWS_AS(a + b, std::logic_error);
            }
        }

        SECTION("mixed static with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 1, dynamic>{{3, 5, 7}};
                auto b = Matrix<int, dynamic, 3>{{10, 100, 1000}};
                auto expected = Matrix<int, 1, dynamic>{{13, 105, 1007}};

                REQUIRE(a + b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, 1, dynamic>{{3, 5}};
                auto b = Matrix<int, dynamic, 3>{{10, 100, 1000}};

                REQUIRE_THROWS_AS(a + b, std::logic_error);
            }
        }

        SECTION("dynamic with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, dynamic, dynamic>{{1, 2}};
                auto b = Matrix<int, dynamic, dynamic>{{30, 40}};
                auto expected = Matrix<int, dynamic, dynamic>{{31, 42}};

                REQUIRE(a + b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, dynamic, dynamic>{{1, 2}};
                auto b = Matrix<int, dynamic, dynamic>{{30, 40, 50}};

                REQUIRE_THROWS_AS(a + b, std::logic_error);
            }
        }
    }

    SECTION("subtraction")
    {
        SECTION("static with static")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 2, 2>{{{-59, 35}, {3, 0}}};
                auto b = Matrix<int, 2, 2>{{{9, 34}, {29, 2}}};
                auto expected = Matrix<int, 2, 2>{{{-68, 1}, {-26, -2}}};

                REQUIRE(a - b == expected);
            }
        }

        SECTION("static with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 3, 1>{{10, 15, 20}};
                auto b = Matrix<int, dynamic, dynamic>{{120, 105, 130}};
                auto expected = Matrix<int, 3, 1>{{-110, -90, -110}};

                REQUIRE(a - b == expected);

                SECTION("view")
                {
                    auto av = MatrixView<int, dynamic, dynamic>{a};

                    av -= b;

                    REQUIRE(av == a);

                    REQUIRE(av == expected);
                }
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, 3, 1>{{10, 15, 20}};
                auto b = Matrix<int, dynamic, dynamic>{{120, 105}};

                REQUIRE_THROWS_AS(a - b, std::logic_error);
            }
        }

        SECTION("mixed static with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 1, dynamic>{{3, 5, 7}};
                auto b = Matrix<int, dynamic, 3>{{10, 100, 1000}};
                auto expected = Matrix<int, 1, dynamic>{{-7, -95, -993}};

                REQUIRE(a - b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, 1, dynamic>{{3, 5}};
                auto b = Matrix<int, dynamic, 3>{{10, 100, 1000}};

                REQUIRE_THROWS_AS(a - b, std::logic_error);
            }
        }

        SECTION("dynamic with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, dynamic, dynamic>{{1, 2}};
                auto b = Matrix<int, dynamic, dynamic>{{30, 40}};
                auto expected = Matrix<int, dynamic, dynamic>{{-29, -38}};

                REQUIRE(a - b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, dynamic, dynamic>{{1, 2}};
                auto b = Matrix<int, dynamic, dynamic>{{30, 40, 50}};

                REQUIRE_THROWS_AS(a - b, std::logic_error);
            }
        }
    }

    SECTION("multiplication")
    {
        SECTION("static with static")
        {
            auto a = Matrix<int, 2, 3>{{{3, 5, 7}, {11, 13, 17}}};
            auto b = Matrix<int, 3, 1>{{23, 29, 31}};
            auto expected = Matrix<int, 2, 1>{{431, 1157}};

            REQUIRE(a * b == expected);
        }

        SECTION("mixed static with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 2, dynamic>{{{3, 5, 7}, {11, 13, 17}}};
                auto b = Matrix<int, 3, 1>{{23, 29, 31}};
                auto expected = Matrix<int, 2, 1>{{431, 1157}};

                REQUIRE(a * b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, 2, dynamic>{{{3, 5, 7, 7}, {11, 13, 17, 17}}};
                auto b = Matrix<int, 3, 1>{{23, 29, 31}};

                REQUIRE_THROWS_AS(a * b, std::logic_error);
            }
        }

        SECTION("dynamic with dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, dynamic, dynamic>{{{1, 2}, {3, 4}, {5, 6}}};
                auto b = Matrix<int, dynamic, dynamic>{{3, 5}};
                auto expected = Matrix<int, dynamic, dynamic>{{13, 29, 45}};

                REQUIRE(a * b == expected);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, 1, dynamic>{{3, 5}};
                auto b = Matrix<int, dynamic, 2>{{{1, 2}, {3, 4}, {5, 6}}};

                REQUIRE_THROWS_AS(a * b, std::logic_error);
            }
        }
    }

    SECTION("scalar multiplication")
    {
        SECTION("static")
        {
            auto a = Matrix<int, 1, 4>{{1, 2, 3, 4}};
            auto expected = Matrix<int, 1, 4>{{5, 10, 15, 20}};

            REQUIRE(a * 5 == expected);

            SECTION("view")
            {
                auto av = MatrixView<int, 1, 4>{a};

                REQUIRE(av * 5 == expected);
            }

            SECTION("constant view")
            {
                auto av = ConstantMatrixView<int, 1, 4>{a};

                REQUIRE(av * 5 == expected);
            }
        }

        SECTION("dynamic")
        {
            auto a = Matrix<int, dynamic, dynamic>{{{1, 2}, {3, 4}}};
            auto expected = Matrix<int, dynamic, dynamic>{{{3, 6}, {9, 12}}};

            REQUIRE(a * 3 == expected);
        }
    }

    SECTION("magnitude")
    {
        SECTION("static")
        {
            auto a = Matrix<double, 1, 3>{{3.0, 5.0, -2.0}};

            REQUIRE(magnitude(a) == Approx(6.164414));
        }

        SECTION("dynamic")
        {
            SECTION("valid dimensions")
            {
                auto a = Matrix<double, dynamic, dynamic>{{3.0, 5.0, -2.0}};

                REQUIRE(magnitude(a) == Approx(6.164414));
            }

            SECTION("invalid dimensions")
            {
                auto a = Matrix<double, dynamic, dynamic>{{{3.0, 5.0}, {-2.0, 0.0}}};

                REQUIRE_THROWS_AS(magnitude(a), std::logic_error);
            }
        }
    }

    SECTION("normalization")
    {
        SECTION("static")
        {
            auto a = Matrix<double, 1, 3>{{3.0, 5.0, -2.0}};

            auto n = normalized(a);

            REQUIRE(n.x() == Approx(0.486664263));

            REQUIRE(n.y() == Approx(0.811107106));

            REQUIRE(n.z() == Approx(-0.324442842));
        }

        SECTION("dynamic")
        {
            SECTION("valid dimensions")
            {
                auto a = Matrix<double, dynamic, dynamic>{{3.0, 5.0, -2.0}};

                auto n = normalized(a);

                REQUIRE(n.x() == Approx(0.486664263));

                REQUIRE(n.y() == Approx(0.811107106));

                REQUIRE(n.z() == Approx(-0.324442842));
            }

            SECTION("invalid dimensions")
            {
                auto a = Matrix<double, dynamic, dynamic>{{{3.0, 5.0}, {-2.0, 0.0}}};

                REQUIRE_THROWS_AS(normalized(a), std::logic_error);
            }
        }
    }

    SECTION("dot product")
    {
        SECTION("static")
        {
            auto a = Matrix<int, 1, 3>{{3, 5, 7}};
            auto b = Matrix<int, 3, 1>{{10, 100, 1000}};

            REQUIRE(dotProduct(a, b) == 7530);

            SECTION("view")
            {
                auto av = MatrixView<int, 1, 3>{a};
                auto bv = MatrixView<int, 3, 1>{b};

                REQUIRE(dotProduct(av, bv) == 7530);
            }

            SECTION("constant view")
            {
                auto av = ConstantMatrixView<int, 1, 3>{a};
                auto bv = ConstantMatrixView<int, 3, 1>{b};

                REQUIRE(dotProduct(av, bv) == 7530);
            }
        }

        SECTION("dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, 1, dynamic>{{3, 5, 7}};
                auto b = Matrix<int, dynamic, 1>{{10, 100, 1000}};

                REQUIRE(dotProduct(a, b) == 7530);

                SECTION("view")
                {
                    auto av = MatrixView<int, 1, dynamic>{a};
                    auto bv = MatrixView<int, dynamic, 1>{b};

                    REQUIRE(dotProduct(av, bv) == 7530);
                }

                SECTION("constant view")
                {
                    auto av = ConstantMatrixView<int, 1, dynamic>{a};
                    auto bv = ConstantMatrixView<int, dynamic, 1>{b};

                    REQUIRE(dotProduct(av, bv) == 7530);
                }
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, dynamic, dynamic>{{{3, 5}, {7, 0}}};
                auto b = Matrix<int, dynamic, dynamic>{{10, 100}};
                auto c = Matrix<int, dynamic, dynamic>{{10, 100, 1000}};

                REQUIRE_THROWS_AS(dotProduct(a, b), std::logic_error);

                REQUIRE_THROWS_AS(dotProduct(b, c), std::logic_error);
            }
        }
    }

    SECTION("cross product")
    {
        SECTION("static")
        {
            auto a = Matrix<int, 1, 3>{{1, 0, 0}};
            auto b = Matrix<int, 3, 1>{{0, 0, 1}};
            auto c = Matrix<int, 3, 1>{{0, -1, 0}};

            REQUIRE(crossProduct(a, b) == c);
        }

        SECTION("dynamic")
        {
            SECTION("dimensions match")
            {
                auto a = Matrix<int, dynamic, dynamic>{{0, 1, 0}};
                auto b = Matrix<int, dynamic, dynamic>{{0, 0, 1}};
                auto c = Matrix<int, 3, 1>{{1, 0, 0}};

                REQUIRE(crossProduct(a, b) == c);
            }

            SECTION("dimensions mismatch")
            {
                auto a = Matrix<int, dynamic, 3>{{{3, 5, 7}, {11, 13, 17}}};
                auto b = Matrix<int, 1, dynamic>{{10, 100, 3}};
                auto c = Matrix<int, 1, dynamic>{{10, 100}};

                REQUIRE_THROWS_AS(crossProduct(a, b), std::logic_error);

                REQUIRE_THROWS_AS(crossProduct(b, c), std::logic_error);
            }
        }
    }

    SECTION("slicing")
    {
        SECTION("data to view")
        {
            auto matrix = Matrix<int, 2, 3>{{{3, 5, 7}, {11, 13, 17}}};

            const auto view = Slicer<0, 0, 2, 2>::slice(matrix);

            REQUIRE(view == Matrix<int, 2, 2>{{{3, 5}, {11, 13}}});

            view += Matrix<int, 2, 2>{{{-10, -20}, {-30, -40}}};

            REQUIRE(view == Matrix<int, 2, 2>{{{-7, -15}, {-19, -27}}});

            REQUIRE(matrix == Matrix<int, 2, 3>{{{-7, -15, 7}, {-19, -27, 17}}});

            SECTION("view to view")
            {
                const auto secondView = Slicer<1, 0, 1, 2>::slice(view);

                secondView *= 5;

                REQUIRE(secondView == Matrix<int, 1, 2>{{-95, -135}});

                REQUIRE(matrix == Matrix<int, 2, 3>{{{-7, -15, 7}, {-95, -135, 17}}});
            }
        }

        SECTION("constant data to constant view")
        {
            const auto matrix = Matrix<int, dynamic, 3>{{{3, 5, 7}, {11, 13, 17}}};

            const auto view = Slicer<0, 1, 2, 2>::slice(matrix);

            REQUIRE(view == Matrix<int, 2, 2>{{{5, 7}, {13, 17}}});

            SECTION("constant view to constant view")
            {
                const auto secondView = Slicer<0, 1, dynamic, dynamic>::slice(view, 2, 1);

                REQUIRE(secondView == Matrix<int, dynamic, dynamic>{{7, 17}});
            }
        }
    }
}
