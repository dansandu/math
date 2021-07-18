#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/common_matrix.hpp"
#include "dansandu/math/internal/constant_matrix_view.hpp"
#include "dansandu/math/internal/heap_matrix.hpp"
#include "dansandu/math/internal/matrix_view.hpp"
#include "dansandu/math/internal/stack_matrix.hpp"

#include <algorithm>

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N, DataStorageStrategy S>
class MatrixImplementation : private DataStorage<T, M, N, S>
{
public:
    static_assert(M >= 0 || M == dynamic, "matrix static rows must be positive or dynamic");
    static_assert(N >= 0 || N == dynamic, "matrix static columns must be positive or dynamic");
    static_assert((M == 0) == (N == 0), "zero static rows imply zero static columns and vice-versa");

    using value_type = T;

    static constexpr auto staticRowCount = M;

    static constexpr auto staticColumnCount = N;

    static constexpr auto dataStorageStrategy = S;

    using DataStorage<T, M, N, S>::DataStorage;

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             std::enable_if_t<isData(S) && dimensionsMatch(M, N, MM, NN) && (M != MM || N != NN || S != SS), int> = 0>
    explicit MatrixImplementation(const MatrixImplementation<T, MM, NN, SS>& other)
        : DataStorage<T, M, N, S>{other.rowCount(), other.columnCount(), other.cbegin(), other.cend()}
    {
    }

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             std::enable_if_t<isView(S) && !isConstantView(SS) && dimensionsMatch(M, N, MM, NN) &&
                                  (M != MM || N != NN || S != SS),
                              int> = 0>
    MatrixImplementation(MatrixImplementation<T, MM, NN, SS>& other)
        : DataStorage<T, M, N, S>{other.rowCount(), other.columnCount(), other.sourceRowCount(),
                                  other.sourceColumnCount(), other.data()}
    {
    }

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             std::enable_if_t<isConstantView(S) && dimensionsMatch(M, N, MM, NN) && (M != MM || N != NN || S != SS),
                              int> = 0>
    MatrixImplementation(const MatrixImplementation<T, MM, NN, SS>& other)
        : DataStorage<T, M, N, S>{other.rowCount(), other.columnCount(), other.sourceRowCount(),
                                  other.sourceColumnCount(), other.data()}
    {
    }

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             typename = std::enable_if_t<isView(S) && dimensionsMatch(M, N, MM, NN)>>
    void deepCopy(const MatrixImplementation<T, MM, NN, SS>& other) const
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::logic_error, "cannot copy matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::copy(other.cbegin(), other.cend(), begin());
    }

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             typename = std::enable_if_t<isData(S) && dimensionsMatch(M, N, MM, NN)>>
    auto& operator+=(const MatrixImplementation<T, MM, NN, SS>& other)
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

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             typename = std::enable_if_t<isView(S) && dimensionsMatch(M, N, MM, NN)>>
    auto& operator+=(const MatrixImplementation<T, MM, NN, SS>& other) const
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

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             typename = std::enable_if_t<isData(S) && dimensionsMatch(M, N, MM, NN)>>
    auto& operator-=(const MatrixImplementation<T, MM, NN, SS>& other)
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

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             typename = std::enable_if_t<isView(S) && dimensionsMatch(M, N, MM, NN)>>
    auto& operator-=(const MatrixImplementation<T, MM, NN, SS>& other) const
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

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto& operator*=(T scalar)
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::MultiplyBy<T>{scalar});
        return *this;
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    auto& operator*=(T scalar) const
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::MultiplyBy<T>{scalar});
        return *this;
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto& operator/=(T scalar)
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::DivideBy<T>{scalar});
        return *this;
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    auto& operator/=(T scalar) const
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::DivideBy<T>{scalar});
        return *this;
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S) && !isNullMatrix(M, N), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isView(S) && !isNullMatrix(M, N), TT>>
    auto& operator()(size_type row, size_type column) const
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

    template<typename TT = T, typename = std::enable_if_t<!isView(S) && !isNullMatrix(M, N), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isData(S) && isVector(M, N), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isView(S) && isVector(M, N), TT>>
    auto& operator()(size_type coordinate) const
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

    template<typename TT = T, typename = std::enable_if_t<!isView(S) && isVector(M, N), TT>>
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
        return DataStorage<T, M, N, S>::rowCount();
    }

    auto columnCount() const
    {
        return DataStorage<T, M, N, S>::columnCount();
    }

    auto sourceRowCount() const
    {
        return DataStorage<T, M, N, S>::sourceRowCount();
    }

    auto sourceColumnCount() const
    {
        return DataStorage<T, M, N, S>::sourceColumnCount();
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
            THROW(std::logic_error, "cannot get the length of a matrix that is not a vector");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto& unsafeSubscript(size_type row, size_type column)
    {
        return DataStorage<T, M, N, S>::unsafeSubscript(row, column);
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    auto& unsafeSubscript(size_type row, size_type column) const
    {
        return DataStorage<T, M, N, S>::unsafeSubscript(row, column);
    }

    template<typename TT = T, typename = std::enable_if_t<!isView(S), TT>>
    const auto& unsafeSubscript(size_type row, size_type column) const
    {
        return DataStorage<T, M, N, S>::unsafeSubscript(row, column);
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto& unsafeSubscript(size_type coordinate)
    {
        return DataStorage<T, M, N, S>::unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    auto& unsafeSubscript(size_type coordinate) const
    {
        return DataStorage<T, M, N, S>::unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<!isView(S), TT>>
    const auto& unsafeSubscript(size_type coordinate) const
    {
        return DataStorage<T, M, N, S>::unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S) && isVectorOfMinimumLength(M, N, 1), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isView(S) && isVectorOfMinimumLength(M, N, 1), TT>>
    auto& x() const
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

    template<typename TT = T, typename = std::enable_if_t<!isView(S) && isVectorOfMinimumLength(M, N, 1), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isData(S) && isVectorOfMinimumLength(M, N, 2), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isView(S) && isVectorOfMinimumLength(M, N, 2), TT>>
    auto& y() const
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

    template<typename TT = T, typename = std::enable_if_t<!isView(S) && isVectorOfMinimumLength(M, N, 2), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isData(S) && isVectorOfMinimumLength(M, N, 3), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isView(S) && isVectorOfMinimumLength(M, N, 3), TT>>
    auto& z() const
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

    template<typename TT = T, typename = std::enable_if_t<!isView(S) && isVectorOfMinimumLength(M, N, 3), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isData(S) && isVectorOfMinimumLength(M, N, 4), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isView(S) && isVectorOfMinimumLength(M, N, 4), TT>>
    auto& w() const
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

    template<typename TT = T, typename = std::enable_if_t<!isView(S) && isVectorOfMinimumLength(M, N, 4), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto begin()
    {
        return DataStorage<T, M, N, S>::begin();
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto end()
    {
        return DataStorage<T, M, N, S>::end();
    }

    auto begin() const
    {
        return DataStorage<T, M, N, S>::begin();
    }

    auto end() const
    {
        return DataStorage<T, M, N, S>::end();
    }

    auto cbegin() const
    {
        return DataStorage<T, M, N, S>::cbegin();
    }

    auto cend() const
    {
        return DataStorage<T, M, N, S>::cend();
    }

    template<typename TT = T, typename = std::enable_if_t<isData(S), TT>>
    auto data()
    {
        return DataStorage<T, M, N, S>::data();
    }

    auto data() const
    {
        return DataStorage<T, M, N, S>::data();
    }
};

template<typename T = double, size_type M = dynamic, size_type N = dynamic>
using Matrix = MatrixImplementation<T, M, N, DataStorageStrategyFor<T, M, N>::value>;

template<typename T = double, size_type M = dynamic, size_type N = dynamic>
using MatrixView = MatrixImplementation<T, M, N, DataStorageStrategy::view>;

template<typename T = double, size_type M = dynamic, size_type N = dynamic>
using ConstantMatrixView = MatrixImplementation<T, M, N, DataStorageStrategy::constantView>;

template<typename T, size_type M, size_type N, size_type MM, size_type NN>
using NormalizedMatrix =
    MatrixImplementation<T, M != dynamic ? M : MM, N != dynamic ? N : NN, DataStorageStrategyFor<T, M, N>::value>;

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN) && std::is_integral_v<T>>>
bool operator==(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
{
    return ((a.rowCount() == b.rowCount()) & (a.columnCount() == b.columnCount())) &&
           std::equal(a.cbegin(), a.cend(), b.cbegin());
}

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN) && std::is_integral_v<T>>>
bool operator!=(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
{
    return !(a == b);
}

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<N == MM || N == dynamic || MM == dynamic>>
auto operator*(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
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

template<typename T, size_type M, size_type N, DataStorageStrategy S>
auto operator*(const MatrixImplementation<T, M, N, S>& matrix, T scalar)
{
    auto result = Matrix<T, M, N>{matrix};
    return result *= scalar;
}

template<typename T, size_type M, size_type N, DataStorageStrategy S>
auto operator*(T scalar, const MatrixImplementation<T, M, N, S>& matrix)
{
    auto result = Matrix<T, M, N>{matrix};
    return result *= scalar;
}

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
auto operator+(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
{
    auto result = NormalizedMatrix<T, M, N, MM, NN>{a};
    return result += b;
}

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
auto operator-(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
{
    auto result = NormalizedMatrix<T, M, N, MM, NN>{a};
    return result -= b;
}

template<typename T, size_type M, size_type N, DataStorageStrategy S, typename = std::enable_if_t<isVector(M, N)>>
auto magnitude(const MatrixImplementation<T, M, N, S>& matrix)
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

template<typename T, size_type M, size_type N, DataStorageStrategy S, typename = std::enable_if_t<isVector(M, N)>>
auto normalized(const MatrixImplementation<T, M, N, S>& matrix)
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

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<vectorsOfEqualLength(M, N, MM, NN)>>
auto dotProduct(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
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

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<vectorsOfEqualLength(M, N, MM, NN)>>
auto distance(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
{
    if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
    {
        if (((a.rowCount() != 1) & (a.columnCount() != 1)) | ((b.rowCount() != 1) & (b.columnCount() != 1)) |
            ((a.rowCount() * a.columnCount() != b.rowCount() * b.columnCount())))
        {
            THROW(std::logic_error, "cannot get the distance of matrices ", a.rowCount(), "x", a.columnCount(), " and ",
                  b.rowCount(), "x", b.columnCount());
        }
    }
    auto sum = dansandu::math::common::additiveIdentity<T>;
    auto left = a.cbegin();
    auto leftEnd = a.cend();
    auto right = b.cbegin();
    while (left != leftEnd)
    {
        const auto difference = *left - *right;
        sum += difference * difference;
        ++left;
        ++right;
    }
    return std::sqrt(sum);
}

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<vectorsOfLength3(M, N, MM, NN)>>
auto crossProduct(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b)
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

template<typename T, size_type M, size_type N, DataStorageStrategy S, size_type MM, size_type NN,
         DataStorageStrategy SS, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
bool close(const MatrixImplementation<T, M, N, S>& a, const MatrixImplementation<T, MM, NN, SS>& b, const T epsilon)
{
    return a.rowCount() == b.rowCount() && a.columnCount() == b.columnCount() &&
           std::equal(a.cbegin(), a.cend(), b.cbegin(), b.cend(),
                      [epsilon](auto l, auto r) { return std::abs(l - r) < epsilon; });
}

}
