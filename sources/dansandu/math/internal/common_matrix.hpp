#pragma once

#include "dansandu/math/common.hpp"

#include <vector>

namespace dansandu::math::matrix
{

using size_type = int;

constexpr auto dynamic = -1;

constexpr auto isNullMatrix(size_type m, size_type n)
{
    return m == 0 || n == 0;
}

constexpr auto isVector(size_type m, size_type n)
{
    return m == dynamic || m == 1 || n == dynamic || n == 1;
}

constexpr auto isVectorOfLength(size_type m, size_type n, size_type len)
{
    return ((m == len && n == 1) || (m == 1 && n == len) || (m == dynamic && n == 1) || (m == 1 && n == dynamic) ||
            (m == dynamic && n == len) || (m == len && n == dynamic) || (m == dynamic && n == dynamic));
}

constexpr auto isVectorOfMinimumLength(size_type m, size_type n, size_type len)
{
    return ((m == 1 || m == dynamic) && (n == dynamic || n >= len)) ||
           ((m == dynamic || m >= len) && (n == 1 || n == dynamic));
}

constexpr auto dimensionsMatch(size_type m, size_type n, size_type mm, size_type nn)
{
    return (m == mm || m == dynamic || mm == dynamic) && (n == nn || n == dynamic || nn == dynamic);
}

constexpr auto canSubscript(size_type m, size_type n, size_type row, size_type column)
{
    return (0 <= row) & (row < m) & (0 <= column) & (column < n);
}

constexpr auto canSubscript(size_type m, size_type n, size_type index)
{
    return (0 <= index) & (((m == 1) & (index < n)) | ((index < m) & (n == 1)));
}

constexpr auto vectorsOfLength3(size_type m, size_type n, size_type mm, size_type nn)
{
    return (((m == 1 || m == dynamic) && (n == 3 || n == dynamic)) ||
            ((m == 3 || m == dynamic) && (n == 1 || n == dynamic))) &&
           (((mm == 1 || mm == dynamic) && (nn == 3 || nn == dynamic)) ||
            ((mm == 3 || mm == dynamic) && (nn == 1 || nn == dynamic)));
}

constexpr auto vectorsOfEqualLength(size_type m, size_type n, size_type mm, size_type nn)
{
    return ((m == 1 || m == dynamic) && (mm == 1 || mm == dynamic) && (n == nn || n == dynamic || nn == dynamic)) ||
           ((m == 1 || m == dynamic) && (nn == 1 || nn == dynamic) && (n == mm || n == dynamic || nn == dynamic)) ||
           ((n == 1 || n == dynamic) && (mm == 1 || mm == dynamic) && (m == nn || m == dynamic || nn == dynamic)) ||
           ((n == 1 || n == dynamic) && (nn == 1 || nn == dynamic) && (m == mm || m == dynamic || nn == dynamic));
}

enum class DataStorageStrategy
{
    stack,
    heap
};

template<typename T, size_type M, size_type N>
struct DataStorageStrategyFor
{
    constexpr static auto stackBytes = static_cast<size_type>(32);

    constexpr static auto value = (M == dynamic || N == dynamic || M * N * sizeof(T) > stackBytes)
                                      ? DataStorageStrategy::heap
                                      : DataStorageStrategy::stack;
};

template<typename T, size_type M, size_type N>
struct DimensionalityStorage;

template<typename T, size_type M, size_type N>
struct DimensionalityStorage
{
    DimensionalityStorage()
    {
    }

    DimensionalityStorage(size_type, size_type)
    {
    }

    auto rowCount() const
    {
        return M;
    }

    auto columnCount() const
    {
        return N;
    }
};

template<typename T, size_type N>
struct DimensionalityStorage<T, dynamic, N>
{
    DimensionalityStorage() : rows{0}
    {
    }

    DimensionalityStorage(size_type r, size_type) : rows{r}
    {
    }

    auto rowCount() const
    {
        return rows;
    }

    auto columnCount() const
    {
        return N;
    }

    size_type rows;
};

template<typename T, size_type M>
struct DimensionalityStorage<T, M, dynamic>
{
    DimensionalityStorage() : columns{0}
    {
    }

    DimensionalityStorage(size_type, size_type c) : columns{c}
    {
    }

    auto rowCount() const
    {
        return M;
    }

    auto columnCount() const
    {
        return columns;
    }

    size_type columns;
};

template<typename T>
struct DimensionalityStorage<T, dynamic, dynamic>
{
    DimensionalityStorage() : rows{0}, columns{0}
    {
    }

    DimensionalityStorage(size_type r, size_type c) : rows{r}, columns{c}
    {
    }

    auto rowCount() const
    {
        return rows;
    }

    auto columnCount() const
    {
        return columns;
    }

    size_type rows;
    size_type columns;
};

template<typename T, size_type M, size_type N, DataStorageStrategy S = DataStorageStrategyFor<T, M, N>::value>
class DataStorage;

}
