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

template<typename Type = double, size_type M = dynamic, size_type N = dynamic>
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
            std::fill(DataStorage<Type, M, N>::data.begin(), DataStorage<Type, M, N>::data.end(),
                      dansandu::math::common::additive_identity<value_type>);
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
            std::copy(array, array + len, DataStorage<Type, M, N>::data.begin());
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

    auto begin() const
    {
        return DataStorage<Type, M, N>::data.cbegin();
    }

    auto end() const
    {
        return DataStorage<Type, M, N>::data.cend();
    }

    auto cbegin() const
    {
        return DataStorage<Type, M, N>::data.cbegin();
    }

    auto cend() const
    {
        return DataStorage<Type, M, N>::data.cend();
    }
};

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<std::is_integral_v<Type>>>
auto operator==(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    return ((a.rowCount() == b.rowCount()) & (a.columnCount() == b.columnCount())) &&
           std::equal(a.cbegin(), a.cend(), b.cbegin());
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<std::is_integral_v<Type>>>
auto operator!=(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    return !(a == b);
}

}
