#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/common_matrix.hpp"
#include "dansandu/math/internal/constant_matrix_view.hpp"
#include "dansandu/math/internal/heap_matrix.hpp"
#include "dansandu/math/internal/matrix_view.hpp"
#include "dansandu/math/internal/stack_matrix.hpp"

namespace dansandu::math::matrix
{

template<typename T = float, size_type M = dynamic, size_type N = dynamic>
class Matrix : private DataStorage<T, M, N>
{
public:
    static_assert(M >= 0 || M == dynamic, "matrix static rows must be positive or dynamic");
    static_assert(N >= 0 || N == dynamic, "matrix static columns must be positive or dynamic");
    static_assert((M == 0) == (N == 0), "zero static rows imply zero static columns and vice-versa");

    using DataStorage<T, M, N>::DataStorage;

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    auto& operator+=(ConstantMatrixView<T, MM, NN> other)
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::logic_error, "cannot add matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::transform(cbegin(), cend(), other.cbegin(), begin(), dansandu::math::common::Add{});
        return *this;
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    auto& operator-=(ConstantMatrixView<T, MM, NN> other)
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::logic_error, "cannot subtract matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::transform(cbegin(), cend(), other.cbegin(), begin(), dansandu::math::common::Subtract{});
        return *this;
    }

    auto& operator*=(T scalar)
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::MultiplyBy<T>{scalar});
        return *this;
    }

    template<typename TT = T, typename = std::enable_if_t<!isNullMatrix(M, N), TT>>
    auto& operator()(size_type row, size_type column)
    {
        if (canSubscript(rowCount(), columnCount(), row, column))
        {
            return unsafeSubscript(row, column);
        }
        else
        {
            THROW(std::out_of_range, "cannot index the (", row, ", ", column, ") element in a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<!isNullMatrix(M, N), TT>>
    const auto& operator()(size_type row, size_type column) const
    {
        if (canSubscript(rowCount(), columnCount(), row, column))
        {
            return unsafeSubscript(row, column);
        }
        else
        {
            THROW(std::out_of_range, "cannot index the (", row, ", ", column, ") element in a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVector(M, N), TT>>
    auto& operator()(size_type coordinate)
    {
        if (canSubscript(rowCount(), columnCount(), coordinate))
        {
            return unsafeSubscript(coordinate);
        }
        else
        {
            THROW(std::out_of_range, "cannot index the ", coordinate, "th element of a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVector(M, N), TT>>
    const auto& operator()(size_type coordinate) const
    {
        if (canSubscript(rowCount(), columnCount(), coordinate))
        {
            return unsafeSubscript(coordinate);
        }
        else
        {
            THROW(std::out_of_range, "cannot index the ", coordinate, "th element of a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    auto rowCount() const
    {
        return DataStorage<T, M, N>::rowCount();
    }

    auto columnCount() const
    {
        return DataStorage<T, M, N>::columnCount();
    }

    template<typename TT = T, typename = std::enable_if_t<isVector(M, N), TT>>
    auto length() const
    {
        if (rowCount() == 1 || columnCount() == 1)
        {
            return rowCount() * columnCount();
        }
        else
        {
            THROW(std::logic_error, "cannot get the length of non-vector matrix");
        }
    }

    auto& unsafeSubscript(size_type row, size_type column)
    {
        return DataStorage<T, M, N>::unsafeSubscript(row, column);
    }

    const auto& unsafeSubscript(size_type row, size_type column) const
    {
        return DataStorage<T, M, N>::unsafeSubscript(row, column);
    }

    auto& unsafeSubscript(size_type coordinate)
    {
        return DataStorage<T, M, N>::unsafeSubscript(coordinate);
    }

    const auto& unsafeSubscript(size_type coordinate) const
    {
        return DataStorage<T, M, N>::unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 1), TT>>
    auto& x()
    {
        if (canSubscript(rowCount(), columnCount(), 0))
        {
            return unsafeSubscript(0);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the x coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 1), TT>>
    const auto& x() const
    {
        if (canSubscript(rowCount(), columnCount(), 0))
        {
            return unsafeSubscript(0);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the x coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 2), TT>>
    auto& y()
    {
        if (canSubscript(rowCount(), columnCount(), 1))
        {
            return unsafeSubscript(1);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the y coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 2), TT>>
    const auto& y() const
    {
        if (canSubscript(rowCount(), columnCount(), 1))
        {
            return unsafeSubscript(1);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the y coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 3), TT>>
    auto& z()
    {
        if (canSubscript(rowCount(), columnCount(), 2))
        {
            return unsafeSubscript(2);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the z coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 3), TT>>
    const auto& z() const
    {
        if (canSubscript(rowCount(), columnCount(), 2))
        {
            return unsafeSubscript(2);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the z coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 4), TT>>
    auto& w()
    {
        if (canSubscript(rowCount(), columnCount(), 3))
        {
            return unsafeSubscript(3);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the w coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 4), TT>>
    const auto& w() const
    {
        if (canSubscript(rowCount(), columnCount(), 3))
        {
            return unsafeSubscript(3);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the w coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    auto begin()
    {
        return DataStorage<T, M, N>::begin();
    }

    auto end()
    {
        return DataStorage<T, M, N>::end();
    }

    auto begin() const
    {
        return DataStorage<T, M, N>::begin();
    }

    auto end() const
    {
        return DataStorage<T, M, N>::end();
    }

    auto cbegin() const
    {
        return begin();
    }

    auto cend() const
    {
        return end();
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    operator MatrixView<T, MM, NN>()
    {
        return {rowCount(), columnCount(), rowCount(), columnCount(), DataStorage<T, M, N>::data()};
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    operator ConstantMatrixView<T, MM, NN>() const
    {
        return {rowCount(), columnCount(), rowCount(), columnCount(), DataStorage<T, M, N>::data()};
    }
};

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN) && std::is_integral_v<T>>>
auto operator==(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    return ((a.rowCount() == b.rowCount()) & (a.columnCount() == b.columnCount())) &&
           std::equal(a.cbegin(), a.cend(), b.cbegin());
}

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN) && std::is_integral_v<T>>>
auto operator!=(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    return !(a == b);
}

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<N == MM || N == dynamic || MM == dynamic>>
auto operator*(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    if constexpr (N == dynamic || MM == dynamic)
    {
        if (a.columnCount() != b.rowCount())
        {
            THROW(std::logic_error, "cannot multiply a ", a.rowCount(), "x", a.columnCount(), " matrix with ",
                  b.rowCount(), "x", b.columnCount(), " -- matrix dimensions do not match");
        }
    }
    auto result = Matrix<T, M, NN>{a.rowCount(), b.columnCount()};
    for (auto m = 0; m < a.rowCount(); ++m)
    {
        for (auto p = 0; p < b.columnCount(); ++p)
        {
            for (auto n = 0; n < a.columnCount(); ++n)
            {
                result.unsafeSubscript(m, p) += a.unsafeSubscript(m, n) * b.unsafeSubscript(n, p);
            }
        }
    }
    return result;
}

template<typename T, size_type M, size_type N>
auto operator*(ConstantMatrixView<T, M, N> matrix, T scalar)
{
    auto result = matrix;
    return result *= scalar;
}

template<typename T, size_type M, size_type N>
auto operator*(T scalar, ConstantMatrixView<T, M, N> matrix)
{
    auto result = matrix;
    return result *= scalar;
}

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
auto operator+(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    auto copy = a;
    return copy += b;
}

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
auto operator-(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    auto copy = a;
    return copy -= b;
}

template<typename T, size_type M, size_type N, typename = std::enable_if_t<isVector(M, N)>>
auto magnitude(ConstantMatrixView<T, M, N> matrix)
{
    if constexpr (M == dynamic || N == dynamic)
    {
        if ((matrix.rowCount() != 1) & (matrix.columnCount() != 1))
        {
            THROW(std::logic_error, "cannot get the magnitude of non-vector matrix ", matrix.rowCount(), "x",
                  matrix.columnCount());
        }
    }
    auto sum = dansandu::math::common::additiveIdentity<T>;
    for (const auto component : matrix)
    {
        sum += component * component;
    }
    return std::sqrt(sum);
}

template<typename T, size_type M, size_type N, typename = std::enable_if_t<isVector(M, N)>>
auto normalized(ConstantMatrixView<T, M, N> matrix)
{
    if constexpr (M == dynamic || N == dynamic)
    {
        if ((matrix.rowCount() != 1) & (matrix.columnCount() != 1))
        {
            THROW(std::logic_error, "cannot get the norm of non-vector matrix ", matrix.rowCount(), "x",
                  matrix.columnCount());
        }
    }
    auto sum = dansandu::math::common::additiveIdentity<T>;
    for (const auto component : matrix)
    {
        sum += component * component;
    }
    return matrix * (dansandu::math::common::multiplicativeIdentity<T> / std::sqrt(sum));
}

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<vectorsOfEqualLength(M, N, MM, NN)>>
auto dotProduct(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
    {
        if (((a.rowCount() != 1) & (a.columnCount() != 1)) | ((b.rowCount() != 1) & (b.columnCount() != 1)) |
            ((a.rowCount() * a.columnCount() != b.rowCount() * b.columnCount())))
        {
            THROW(std::logic_error, "cannot get the dot product of matrices ", a.rowCount(), "x", a.columnCount(),
                  " and ", b.rowCount(), "x", b.columnCount());
        }
    }
    auto sum = dansandu::math::common::additiveIdentity<T>;
    auto left = a.cbegin();
    auto right = b.cbegin();
    while (left != a.cend())
    {
        sum += *left++ * *right++;
    }
    return sum;
}

template<typename T, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<vectorsOfLength3(M, N, MM, NN)>>
auto crossProduct(ConstantMatrixView<T, M, N> a, ConstantMatrixView<T, MM, NN> b)
{
    if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
    {
        if (((a.rowCount() != 1) & (a.columnCount() != 1)) | ((b.rowCount() != 1) & (b.columnCount() != 1)) |
            (a.rowCount() * a.columnCount() != 3) | (b.rowCount() * b.columnCount() != 3))
        {
            THROW(std::logic_error, "cannot get the cross product of matrices ", a.rowCount(), "x", a.columnCount(),
                  " and ", b.rowCount(), "x", b.columnCount());
        }
    }

    const auto x = a.unsafeSubscript(1) * b.unsafeSubscript(2) - b.unsafeSubscript(1) * a.unsafeSubscript(2);
    const auto y = b.unsafeSubscript(0) * a.unsafeSubscript(2) - a.unsafeSubscript(0) * b.unsafeSubscript(2);
    const auto z = a.unsafeSubscript(0) * b.unsafeSubscript(1) - b.unsafeSubscript(0) * a.unsafeSubscript(1);

    return Matrix<T, 3, 1>{{x, y, z}};
}

}
