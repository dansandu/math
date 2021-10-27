#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/matrix/common.hpp"
#include "dansandu/math/internal/matrix/data_storage_constant_view.hpp"
#include "dansandu/math/internal/matrix/data_storage_heap.hpp"
#include "dansandu/math/internal/matrix/data_storage_stack.hpp"
#include "dansandu/math/internal/matrix/data_storage_view.hpp"

#include <algorithm>

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N, DataStorageStrategy S>
class MatrixImplementation
{
public:
    static_assert(M >= 0 || M == dynamic, "matrix static rows must be positive or dynamic");
    static_assert(N >= 0 || N == dynamic, "matrix static columns must be positive or dynamic");
    static_assert((M == 0) == (N == 0), "zero static rows imply zero static columns and vice-versa");

    using value_type = T;
    using iterator = typename DataStorage<T, M, N, S>::iterator;
    using const_iterator = typename DataStorage<T, M, N, S>::const_iterator;

    static constexpr auto staticRowCount = M;

    static constexpr auto staticColumnCount = N;

    static constexpr auto dataStorageStrategy = S;

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    MatrixImplementation()
    {
    }

    template<size_type L, typename = std::enable_if_t<isContainer(S) && isVectorOfLength(M, N, L)>>
    explicit MatrixImplementation(const T (&array)[L]) : dataStorage_{array}
    {
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<isContainer(S) && dimensionsMatch(M, N, MM, NN)>>
    explicit MatrixImplementation(const T (&array)[MM][NN]) : dataStorage_{array}
    {
    }

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    MatrixImplementation(size_type rows, size_type columns,
                         const T& fillValue = dansandu::math::common::additiveIdentity<T>)
        : dataStorage_{rows, columns, fillValue}
    {
    }

    template<typename TT = T, typename = std::enable_if_t<isHeapContainer(S), TT>>
    MatrixImplementation(size_type rows, size_type columns, std::vector<T> buffer)
        : dataStorage_{rows, columns, std::move(buffer)}
    {
    }

    template<typename IteratorBegin, typename IteratorEnd, typename TT = T,
             typename = std::enable_if_t<isContainer(S), TT>>
    MatrixImplementation(size_type rows, size_type columns, IteratorBegin sourceBegin, IteratorEnd sourceEnd)
        : dataStorage_{rows, columns, sourceBegin, sourceEnd}
    {
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    MatrixImplementation(size_type viewRowCount, size_type viewColumnCount, size_type sourceRowCount,
                         size_type sourceColumnCount, T* viewBegin)
        : dataStorage_{viewRowCount, viewColumnCount, sourceRowCount, sourceColumnCount, viewBegin}
    {
    }

    template<typename TT = T, typename = std::enable_if_t<isConstantView(S), TT>>
    MatrixImplementation(size_type viewRowCount, size_type viewColumnCount, size_type sourceRowCount,
                         size_type sourceColumnCount, const T* viewBegin)
        : dataStorage_{viewRowCount, viewColumnCount, sourceRowCount, sourceColumnCount, viewBegin}
    {
    }

    template<
        size_type MM, size_type NN, DataStorageStrategy SS,
        std::enable_if_t<isContainer(S) && dimensionsMatch(M, N, MM, NN) && (M != MM || N != NN || S != SS), int> = 0>
    explicit MatrixImplementation(const MatrixImplementation<T, MM, NN, SS>& other)
        : dataStorage_{other.rowCount(), other.columnCount(), other.cbegin(), other.cend()}
    {
    }

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             std::enable_if_t<isView(S) && !isConstantView(SS) && dimensionsMatch(M, N, MM, NN) &&
                                  (M != MM || N != NN || S != SS),
                              int> = 0>
    MatrixImplementation(MatrixImplementation<T, MM, NN, SS>& other)
        : dataStorage_{other.rowCount(), other.columnCount(), other.sourceRowCount(), other.sourceColumnCount(),
                       other.data()}
    {
    }

    template<size_type MM, size_type NN, DataStorageStrategy SS,
             std::enable_if_t<isConstantView(S) && dimensionsMatch(M, N, MM, NN) && (M != MM || N != NN || S != SS),
                              int> = 0>
    MatrixImplementation(const MatrixImplementation<T, MM, NN, SS>& other)
        : dataStorage_{other.rowCount(), other.columnCount(), other.sourceRowCount(), other.sourceColumnCount(),
                       other.data()}
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
             typename = std::enable_if_t<isContainer(S) && dimensionsMatch(M, N, MM, NN)>>
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
             typename = std::enable_if_t<isContainer(S) && dimensionsMatch(M, N, MM, NN)>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
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

    auto operator-() const;

    template<typename TT = T, typename = std::enable_if_t<isContainer(S) && !isNullMatrix(M, N), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S) && isVector(M, N), TT>>
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
        return dataStorage_.rowCount();
    }

    auto columnCount() const
    {
        return dataStorage_.columnCount();
    }

    auto sourceRowCount() const
    {
        return dataStorage_.sourceRowCount();
    }

    auto sourceColumnCount() const
    {
        return dataStorage_.sourceColumnCount();
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    auto& unsafeSubscript(size_type row, size_type column)
    {
        return dataStorage_.unsafeSubscript(row, column);
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    auto& unsafeSubscript(size_type row, size_type column) const
    {
        return dataStorage_.unsafeSubscript(row, column);
    }

    template<typename TT = T, typename = std::enable_if_t<!isView(S), TT>>
    const auto& unsafeSubscript(size_type row, size_type column) const
    {
        return dataStorage_.unsafeSubscript(row, column);
    }

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    auto& unsafeSubscript(size_type coordinate)
    {
        return dataStorage_.unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<isView(S), TT>>
    auto& unsafeSubscript(size_type coordinate) const
    {
        return dataStorage_.unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<!isView(S), TT>>
    const auto& unsafeSubscript(size_type coordinate) const
    {
        return dataStorage_.unsafeSubscript(coordinate);
    }

    template<typename TT = T, typename = std::enable_if_t<isContainer(S) && isVectorOfMinimumLength(M, N, 1), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S) && isVectorOfMinimumLength(M, N, 2), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S) && isVectorOfMinimumLength(M, N, 3), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S) && isVectorOfMinimumLength(M, N, 4), TT>>
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

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    auto begin()
    {
        return dataStorage_.begin();
    }

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    auto end()
    {
        return dataStorage_.end();
    }

    auto begin() const
    {
        return dataStorage_.begin();
    }

    auto end() const
    {
        return dataStorage_.end();
    }

    auto cbegin() const
    {
        return dataStorage_.cbegin();
    }

    auto cend() const
    {
        return dataStorage_.cend();
    }

    template<typename TT = T, typename = std::enable_if_t<isContainer(S), TT>>
    auto data()
    {
        return dataStorage_.data();
    }

    auto data() const
    {
        return dataStorage_.data();
    }

private:
    DataStorage<T, M, N, S> dataStorage_;
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
auto MatrixImplementation<T, M, N, S>::operator-() const
{
    auto copy = Matrix<T, M, N>{*this};
    std::transform(cbegin(), cend(), copy.begin(),
                   dansandu::math::common::MultiplyBy<T>{-dansandu::math::common::multiplicativeIdentity<T>});
    return copy;
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

template<typename T, size_type M, size_type N, DataStorageStrategy S>
auto operator/(const MatrixImplementation<T, M, N, S>& matrix, const T& scalar)
{
    auto result = Matrix<T, M, N>{matrix};
    return result /= scalar;
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

template<typename T, size_type M, size_type N, DataStorageStrategy S>
std::ostream& operator<<(std::ostream& stream, const MatrixImplementation<T, M, N, S>& matrix)
{
    auto printRow = [&](auto row)
    {
        stream << "{";
        if (matrix.columnCount() > 0)
        {
            stream << matrix.unsafeSubscript(row, 0);
        }
        for (auto column = 1; column < matrix.columnCount(); ++column)
        {
            stream << ", " << matrix.unsafeSubscript(row, column);
        }
        stream << "}";
    };

    stream << "{";
    if (matrix.rowCount() > 0)
    {
        printRow(0);
    }
    for (auto row = 1; row < matrix.rowCount(); ++row)
    {
        stream << ", ";
        printRow(row);
    }
    return stream << "}";
}

}
