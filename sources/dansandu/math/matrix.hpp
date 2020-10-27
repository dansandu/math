#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"

#include <array>
#include <vector>

namespace dansandu::math::matrix
{

using dansandu::math::common::dynamic;

using dansandu::math::common::size_type;

enum class DataStorageStrategy
{
    stack,
    heap
};

template<typename Type, size_type M, size_type N>
struct DataStorageStrategyFor
{
    constexpr static auto stackBytes = static_cast<size_type>(32);

    constexpr static auto value = (M == dynamic || N == dynamic || M * N * sizeof(Type) > stackBytes)
                                      ? DataStorageStrategy::heap
                                      : DataStorageStrategy::stack;
};

template<typename Type, size_type M, size_type N,
         DataStorageStrategy dataStorageStrategy = DataStorageStrategyFor<Type, M, N>::value>
struct DataStorage;

template<typename Type, size_type M, size_type N>
struct DataStorage<Type, M, N, DataStorageStrategy::stack>
{
    constexpr static auto strategy = DataStorageStrategy::stack;

    std::array<Type, M * N> data;
};

template<typename Type, size_type M, size_type N>
struct DataStorage<Type, M, N, DataStorageStrategy::heap>
{
    constexpr static auto strategy = DataStorageStrategy::heap;

    std::vector<Type> data;
};

template<typename Type, size_type M, size_type N>
struct DimensionalityStorage;

template<typename Type>
struct DimensionalityStorage<Type, dynamic, dynamic>
{
    constexpr DimensionalityStorage() noexcept : rows{0}, columns{0}
    {
    }

    constexpr DimensionalityStorage(size_type r, size_type c) noexcept : rows{r}, columns{c}
    {
    }

    size_type rows;
    size_type columns;
};

template<typename Type, size_type N>
struct DimensionalityStorage<Type, dynamic, N>
{
    constexpr DimensionalityStorage() noexcept : rows{0}
    {
    }

    constexpr DimensionalityStorage(size_type r, size_type) noexcept : rows{r}
    {
    }

    size_type rows;
};

template<typename Type, size_type M>
struct DimensionalityStorage<Type, M, dynamic>
{
    constexpr DimensionalityStorage() noexcept : columns{0}
    {
    }

    constexpr DimensionalityStorage(size_type, size_type c) noexcept : columns{c}
    {
    }

    size_type columns;
};

template<typename Type, size_type M, size_type N>
struct DimensionalityStorage
{
    constexpr DimensionalityStorage() noexcept
    {
    }

    constexpr DimensionalityStorage(size_type, size_type) noexcept
    {
    }
};

template<typename Type = float, size_type M = dynamic, size_type N = dynamic>
class Matrix : private DimensionalityStorage<Type, M, N>, private DataStorage<Type, M, N>
{
    constexpr static auto isVectorOfMinimumLength(size_type length) noexcept
    {
        return ((M == 1 || M == dynamic) && (N == dynamic || N >= length)) ||
               ((M == dynamic || M >= length) && (N == 1 || N == dynamic));
    }

    constexpr auto canSubscript(size_type index) const noexcept
    {
        return ((rowCount() == 1) & (index < columnCount())) | ((columnCount() == 1) & (index < rowCount()));
    }

    constexpr auto canSubscript(size_type row, size_type column) const noexcept
    {
        return (row >= 0) & (row < rowCount()) & (column >= 0) & (column < columnCount());
    }

    constexpr auto getIndex(size_type index) const noexcept
    {
        return index;
    }

    constexpr auto getIndex(size_type row, size_type column) const noexcept
    {
        return row * columnCount() + column;
    }

public:
    static_assert(M >= 0 || M == dynamic, "matrix static rows must be positive or dynamic");
    static_assert(N >= 0 || N == dynamic, "matrix static columns must be positive or dynamic");
    static_assert((M == 0) == (N == 0), "zero static rows imply zero static columns and vice-versa");

    using value_type = Type;

    constexpr Matrix() noexcept(DataStorage<Type, M, N>::strategy == DataStorageStrategy::stack)
    {
        if constexpr (DataStorage<Type, M, N>::strategy == DataStorageStrategy::stack)
        {
            std::fill(begin(), end(), dansandu::math::common::additive_identity<value_type>);
        }
        else if constexpr (M != dynamic && N != dynamic)
        {
            DataStorage<Type, M, N>::data =
                std::vector<value_type>(M * N, dansandu::math::common::additive_identity<value_type>);
        }
    }

    template<size_type len, typename T = Type,
             typename = std::enable_if_t<(M == len && N == 1) || (M == 1 && N == len) || (M == dynamic && N == 1) ||
                                             (M == 1 && N == dynamic) || (M == dynamic && N == len) ||
                                             (M == len && N == dynamic) || (M == dynamic && N == dynamic),
                                         T>>
    explicit constexpr Matrix(const value_type (&array)[len]) noexcept(DataStorage<Type, M, N>::strategy ==
                                                                       DataStorageStrategy::stack)
    {
        if constexpr (M == dynamic)
        {
            if constexpr (N == len)
            {
                DimensionalityStorage<Type, M, N>::rows = 1;
            }
            else
            {
                DimensionalityStorage<Type, M, N>::rows = len;
            }
        }

        if constexpr (N == dynamic)
        {
            if constexpr (M == 1)
            {
                DimensionalityStorage<Type, M, N>::columns = len;
            }
            else
            {
                DimensionalityStorage<Type, M, N>::columns = 1;
            }
        }

        if constexpr (DataStorage<Type, M, N>::strategy == DataStorageStrategy::stack)
        {
            std::copy(array, array + len, begin());
        }
        else
        {
            DataStorage<Type, M, N>::data = std::vector<value_type>{array, array + len};
        }
    }

    template<size_type m, size_type n, typename T = Type,
             typename = std::enable_if_t<(M == m && N == n) || (M == m && N == dynamic) || (M == dynamic && N == n) ||
                                             (M == dynamic && N == dynamic),
                                         T>>
    explicit constexpr Matrix(const value_type (&array)[m][n]) noexcept(DataStorage<Type, M, N>::strategy ==
                                                                        DataStorageStrategy::stack)
        : DimensionalityStorage<Type, M, N>{m, n}
    {
        if constexpr (DataStorage<Type, M, N>::strategy == DataStorageStrategy::heap)
        {
            DataStorage<Type, M, N>::data.reserve(m * n);
        }
        for (auto row = 0; row < m; ++row)
        {
            for (auto column = 0; column < n; ++column)
            {
                if constexpr (DataStorage<Type, M, N>::strategy == DataStorageStrategy::stack)
                {
                    DataStorage<Type, M, N>::data[getIndex(row, column)] = array[row][column];
                }
                else
                {
                    DataStorage<Type, M, N>::data.push_back(array[row][column]);
                }
            }
        }
    }

    constexpr Matrix(
        size_type rows, size_type columns,
        value_type fillValue =
            dansandu::math::common::additive_identity<value_type>) noexcept(DataStorage<Type, M, N>::strategy ==
                                                                            DataStorageStrategy::stack)
        : DimensionalityStorage<Type, M, N>{rows, columns}
    {
        if constexpr (DataStorage<Type, M, N>::strategy == DataStorageStrategy::stack)
        {
            std::fill(begin(), end(), fillValue);
        }
        else if constexpr (M != dynamic && N != dynamic)
        {
            DataStorage<Type, M, N>::data = std::vector<value_type>(M * N, fillValue);
        }
        else
        {
            DataStorage<Type, M, N>::data = std::vector<value_type>(rows * columns, fillValue);
        }
    }

    template<size_type MM, size_type NN, typename T = Type,
             typename = std::enable_if_t<
                 (M == MM || M == dynamic || MM == dynamic) && (N == NN || N == dynamic || NN == dynamic), T>>
    auto& operator+=(const Matrix<value_type, MM, NN>& other)
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::runtime_error, "cannot add matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::transform(cbegin(), cend(), other.cbegin(), begin(), dansandu::math::common::add{});
        return *this;
    }

    template<size_type MM, size_type NN, typename T = Type,
             typename = std::enable_if_t<
                 (M == MM || M == dynamic || MM == dynamic) && (N == NN || N == dynamic || NN == dynamic), T>>
    auto& operator-=(const Matrix<value_type, MM, NN>& other)
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::runtime_error, "cannot subtract matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::transform(cbegin(), cend(), other.cbegin(), begin(), dansandu::math::common::subtract{});
        return *this;
    }

    auto& operator*=(const value_type& scalar)
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::multiplyBy<value_type>{scalar});
        return *this;
    }

    constexpr auto rowCount() const noexcept
    {
        if constexpr (M == dynamic)
        {
            return DimensionalityStorage<Type, M, N>::rows;
        }
        else
        {
            return M;
        }
    }

    constexpr auto columnCount() const noexcept
    {
        if constexpr (N == dynamic)
        {
            return DimensionalityStorage<Type, M, N>::columns;
        }
        else
        {
            return N;
        }
    }

    template<typename T = Type, typename = std::enable_if_t<M == dynamic || M == 1 || N == dynamic || N == 1, T>>
    auto length() const
    {
        if (rowCount() == 1 || columnCount() == 1)
        {
            return rowCount() * columnCount();
        }
        else
        {
            THROW(std::runtime_error, "cannot get the length of non-vector matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<M != 0 && N != 0, T>>
    auto& operator()(size_type row, size_type column)
    {
        if (canSubscript(row, column))
        {
            return DataStorage<Type, M, N>::data[getIndex(row, column)];
        }
        else
        {
            THROW(std::out_of_range, "cannot index the (", row, ", ", column, ") element in a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<M != 0 && N != 0, T>>
    const auto& operator()(size_type row, size_type column) const
    {
        if (canSubscript(row, column))
        {
            return DataStorage<Type, M, N>::data[getIndex(row, column)];
        }
        else
        {
            THROW(std::out_of_range, "cannot index the (", row, ", ", column, ") element in a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<M == dynamic || N == dynamic || M == 1 || N == 1, T>>
    auto& operator()(size_type coordinate)
    {
        if (canSubscript(coordinate))
        {
            return DataStorage<Type, M, N>::data[getIndex(coordinate)];
        }
        else
        {
            THROW(std::out_of_range, "cannot index the ", coordinate, "th element of a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<M == dynamic || N == dynamic || M == 1 || N == 1, T>>
    const auto& operator()(size_type coordinate) const
    {
        if (canSubscript(coordinate))
        {
            return DataStorage<Type, M, N>::data[getIndex(coordinate)];
        }
        else
        {
            THROW(std::out_of_range, "cannot index the ", coordinate, "th element of a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(1), T>>
    auto& x()
    {
        if (canSubscript(0))
        {
            return DataStorage<Type, M, N>::data[getIndex(0)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the x coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(1), T>>
    const auto& x() const
    {
        if (canSubscript(0))
        {
            return DataStorage<Type, M, N>::data[getIndex(0)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the x coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(2), T>>
    auto& y()
    {
        if (canSubscript(1))
        {
            return DataStorage<Type, M, N>::data[getIndex(1)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the y coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(2), T>>
    const auto& y() const
    {
        if (canSubscript(1))
        {
            return DataStorage<Type, M, N>::data[getIndex(1)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the y coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(3), T>>
    auto& z()
    {
        if (canSubscript(2))
        {
            return DataStorage<Type, M, N>::data[getIndex(2)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the z coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(3), T>>
    const auto& z() const
    {
        if (canSubscript(2))
        {
            return DataStorage<Type, M, N>::data[getIndex(2)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the z coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(4), T>>
    auto& w()
    {
        if (canSubscript(3))
        {
            return DataStorage<Type, M, N>::data[getIndex(3)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the w coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename T = Type, typename = std::enable_if_t<isVectorOfMinimumLength(4), T>>
    const auto& w() const
    {
        if (canSubscript(3))
        {
            return DataStorage<Type, M, N>::data[getIndex(3)];
        }
        else
        {
            THROW(std::out_of_range, "cannot get the w coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    auto begin() noexcept
    {
        return DataStorage<Type, M, N>::data.begin();
    }

    auto end() noexcept
    {
        return DataStorage<Type, M, N>::data.end();
    }

    auto begin() const noexcept
    {
        return DataStorage<Type, M, N>::data.cbegin();
    }

    auto end() const noexcept
    {
        return DataStorage<Type, M, N>::data.cend();
    }

    auto cbegin() const noexcept
    {
        return DataStorage<Type, M, N>::data.cbegin();
    }

    auto cend() const noexcept
    {
        return DataStorage<Type, M, N>::data.cend();
    }
};

template<typename Type, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<N == MM || N == dynamic || MM == dynamic, Type>>
auto operator*(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    if constexpr (N == dynamic || MM == dynamic)
    {
        if (a.columnCount() != b.rowCount())
        {
            THROW(std::runtime_error, "cannot multiply a ", a.rowCount(), "x", a.columnCount(), " matrix with ",
                  b.rowCount(), "x", b.columnCount(), " -- matrix dimensions do not match");
        }
    }
    auto result = Matrix<Type, M, NN>{a.rowCount(), b.columnCount()};
    for (auto m = 0; m < a.rowCount(); ++m)
    {
        for (auto p = 0; p < b.columnCount(); ++p)
        {
            for (auto n = 0; n < a.columnCount(); ++n)
            {
                result(m, p) += a(m, n) * b(n, p);
            }
        }
    }
    return result;
}

template<typename Type, size_type M, size_type N>
auto operator*(const Matrix<Type, M, N>& matrix, Type scalar)
{
    auto result = matrix;
    return result *= scalar;
}

template<typename Type, size_type M, size_type N>
auto operator*(Type scalar, const Matrix<Type, M, N>& matrix)
{
    auto result = matrix;
    return result *= scalar;
}

template<typename Type, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<
             (M == MM || M == dynamic || MM == dynamic) && (N == NN || N == dynamic || NN == dynamic), Type>>
auto operator+(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    auto copy = a;
    return copy += b;
}

template<typename Type, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<
             (M == MM || M == dynamic || MM == dynamic) && (N == NN || N == dynamic || NN == dynamic), Type>>
auto operator-(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    auto copy = a;
    return copy -= b;
}

template<typename Type, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<(M == MM || M == dynamic || MM == dynamic) &&
                                     (N == NN || N == dynamic || NN == dynamic) && std::is_integral_v<Type>>>
auto operator==(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    return ((a.rowCount() == b.rowCount()) & (a.columnCount() == b.columnCount())) &&
           std::equal(a.cbegin(), a.cend(), b.cbegin());
}

template<typename Type, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<(M == MM || M == dynamic || MM == dynamic) &&
                                     (N == NN || N == dynamic || NN == dynamic) && std::is_integral_v<Type>>>
auto operator!=(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    return !(a == b);
}

template<typename Type, size_type M, size_type N,
         typename = std::enable_if_t<M == 1 || M == dynamic || N == 1 || N == dynamic, Type>>
auto magnitude(const Matrix<Type, M, N>& matrix)
{
    if constexpr (M == dynamic || N == dynamic)
    {
        if ((matrix.rowCount() != 1) & (matrix.columnCount() != 1))
        {
            THROW(std::runtime_error, "cannot get the magnitude of non-vector matrix ", matrix.rowCount(), "x",
                  matrix.columnCount());
        }
    }
    auto sum = dansandu::math::common::additive_identity<Type>;
    for (const auto component : matrix)
    {
        sum += component * component;
    }
    return std::sqrt(sum);
}

template<typename Type, size_type M, size_type N,
         typename = std::enable_if_t<M == 1 || M == dynamic || N == 1 || N == dynamic, Type>>
auto normalized(const Matrix<Type, M, N>& matrix)
{
    if constexpr (M == dynamic || N == dynamic)
    {
        if ((matrix.rowCount() != 1) & (matrix.columnCount() != 1))
        {
            THROW(std::runtime_error, "cannot get the norm of non-vector matrix ", matrix.rowCount(), "x",
                  matrix.columnCount());
        }
    }
    auto sum = dansandu::math::common::additive_identity<Type>;
    for (const auto component : matrix)
    {
        sum += component * component;
    }
    return matrix * (dansandu::math::common::multiplicative_identity<Type> / std::sqrt(sum));
}

template<
    typename Type, size_type M, size_type N, size_type MM, size_type NN,
    typename = std::enable_if_t<
        ((M == 1 || M == dynamic) && (MM == 1 || MM == dynamic) && (N == NN || N == dynamic || NN == dynamic)) ||
            ((M == 1 || M == dynamic) && (NN == 1 || NN == dynamic) && (N == MM || N == dynamic || NN == dynamic)) ||
            ((N == 1 || N == dynamic) && (MM == 1 || MM == dynamic) && (M == NN || M == dynamic || NN == dynamic)) ||
            ((N == 1 || N == dynamic) && (NN == 1 || NN == dynamic) && (M == MM || M == dynamic || NN == dynamic)),
        Type>>
auto dotProduct(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
    {
        if (((a.rowCount() != 1) & (a.columnCount() != 1)) | ((b.rowCount() != 1) & (b.columnCount() != 1)) |
            ((a.rowCount() * a.columnCount() != b.rowCount() * b.columnCount())))
        {
            THROW(std::runtime_error, "cannot get the dot product of matrices ", a.rowCount(), "x", a.columnCount(),
                  " and ", b.rowCount(), "x", b.columnCount());
        }
    }
    auto sum = dansandu::math::common::additive_identity<Type>;
    auto left = a.cbegin();
    auto right = b.cbegin();
    while (left != a.cend())
    {
        sum += *left++ * *right++;
    }
    return sum;
}

template<typename Type, size_type M, size_type N, size_type MM, size_type NN,
         typename = std::enable_if_t<(((M == 1 || M == dynamic) && (N == 3 || N == dynamic)) ||
                                      ((M == 3 || M == dynamic) && (N == 1 || N == dynamic))) &&
                                         (((MM == 1 || MM == dynamic) && (NN == 3 || NN == dynamic)) ||
                                          ((MM == 3 || MM == dynamic) && (NN == 1 || NN == dynamic))),
                                     Type>>
auto crossProduct(const Matrix<Type, M, N>& a, const Matrix<Type, MM, NN>& b)
{
    if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
    {
        if (((a.rowCount() != 1) & (a.columnCount() != 1)) | ((b.rowCount() != 1) & (b.columnCount() != 1)) |
            (a.rowCount() * a.columnCount() != 3) | (b.rowCount() * b.columnCount() != 3))
        {
            THROW(std::runtime_error, "cannot get the cross product of matrices ", a.rowCount(), "x", a.columnCount(),
                  " and ", b.rowCount(), "x", b.columnCount());
        }
    }
    const auto x = a.y() * b.z() - b.y() * a.z();
    const auto y = b.x() * a.z() - a.x() * b.z();
    const auto z = a.x() * b.y() - b.x() * a.y();
    return Matrix<Type, 3, 1>{{x, y, z}};
}

}
