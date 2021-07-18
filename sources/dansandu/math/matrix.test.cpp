#include "catchorg/catch/catch.hpp"
#include "dansandu/math/matrix.hpp"

#include <stdexcept>

using Catch::Detail::Approx;
using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::crossProduct;
using dansandu::math::matrix::distance;
using dansandu::math::matrix::dotProduct;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::magnitude;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;
using dansandu::math::matrix::normalized;
using dansandu::math::matrix::sliceColumn;
using dansandu::math::matrix::Slicer;
using dansandu::math::matrix::sliceRow;

TEST_CASE("matrix")
{
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

    SECTION("distance")
    {
        auto a = Matrix<double>{{1.0, 3.0, 10.0}};
        auto b = Matrix<double>{{5.0, 0.0, -2.0}};

        REQUIRE(distance(a, b) == Approx(13.0));
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

    SECTION("row slicing")
    {
        auto matrix = Matrix<int>{{{1, 2}, {3, 4}}};

        SECTION("first row")
        {
            const auto row = sliceRow(matrix, 0);

            row.x() = 11;

            REQUIRE(matrix == Matrix<int>{{{11, 2}, {3, 4}}});

            row.y() = 22;

            REQUIRE(matrix == Matrix<int>{{{11, 22}, {3, 4}}});
        }

        SECTION("second row")
        {
            const auto row = sliceRow(matrix, 1);

            row.x() = 33;

            REQUIRE(matrix == Matrix<int>{{{1, 2}, {33, 4}}});

            row.y() = 44;

            REQUIRE(matrix == Matrix<int>{{{1, 2}, {33, 44}}});
        }
    }

    SECTION("column slicing")
    {
        auto matrix = Matrix<int>{{{1, 2}, {3, 4}}};

        SECTION("first column")
        {
            const auto column = sliceColumn(matrix, 0);

            column.x() = 11;

            REQUIRE(matrix == Matrix<int>{{{11, 2}, {3, 4}}});

            column.y() = 33;

            REQUIRE(matrix == Matrix<int>{{{11, 2}, {33, 4}}});
        }

        SECTION("second row")
        {
            const auto column = sliceColumn(matrix, 1);

            column.x() = 22;

            REQUIRE(matrix == Matrix<int>{{{1, 22}, {3, 4}}});

            column.y() = 44;

            REQUIRE(matrix == Matrix<int>{{{1, 22}, {3, 44}}});
        }
    }
}
