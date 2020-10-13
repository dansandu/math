#pragma once

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

template<typename Type, size_type M, size_type N, DataStorageStrategy dataStorageStrategy>
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
    constexpr DimensionalityStorage() noexcept : rows{0}, columns{0} { }

    size_type rows;
    size_type columns;
};

template<typename Type, size_type N>
struct DimensionalityStorage<Type, dynamic, N>
{
    constexpr DimensionalityStorage() noexcept : rows{0} { }

    size_type rows;
};

template<typename Type, size_type M>
struct DimensionalityStorage<Type, M, dynamic>
{
    constexpr DimensionalityStorage() noexcept : columns{0} { }

    size_type columns;
};

template<typename Type, size_type M, size_type N>
struct DimensionalityStorage
{

};

template<typename Type, size_type M, size_type N>
class Matrix : private DimensionalityStorage<Type, M, N>, private DataStorage<Type, M, N>
{
public:
    static_assert(M >= 0 || M == dynamic, "matrix static rows must be positive or dynamic");
    static_assert(N >= 0 || N == dynamic, "matrix static columns must be positive or dynamic");
    static_assert((M == 0) == (N == 0), "zero static rows imply zero static columns and vice-versa");
    
    using value_type = Type;

    Matrix() noexcept(strategy == DataStorageStrategy::stack)
    {
        if constexpr (strategy == DataStorageStrategy::stack)
        {
            std::fill(data.begin(), data.end(), additive_identity<value_type>);
        }
        else if constexpr (M != dynamic && N != dynamic)
        {
            data = std::vector<value_type>(M * N, additive_identity<value_type>);
        }
    }

    template<typename U = Type, size_type L,
             typename = std::enable_if_t<(M == L && N == 1) ||
                                         (M == 1 && N == L) ||
                                         (M == dynamic && N == 1) ||
                                         (M == 1       && N == dynamic) ||
                                         (M == dynamic && N == L) ||
                                         (M == L       && N == dynamic) ||
                                         (M == dynamic && N == dynamic), U>>
    explicit Matrix(const value_type (&array)[L]) noexcept(strategy == DataStorageStrategy::stack)
    {
        if constexpr (M == dynamic)
        {
            if constexpr (N == L)
            {
                rows = 1;
            }
            else
            {
                rows = L;
            } 
        }

        if constexpr (N == dynamic)
        {
            if constexpr (M == 1)
            {
                columns = L;
            }
            else
            {
                columns = 1;
            }
        }

        if constexpr (strategy == DataStorageStrategy::stack)
        {
            std::copy(array, array + L, data.begin());
        }
        else
        {
            data = std::vector<value_type>{array, array + L};
        }
    }

    auto rowCount() const noexcept
    {
        if constexpr (M == dynamic)
        {
            return rows;
        }
        else
        {
            return M;
        }
    }

    auto columnCount() const noexcept
    {
        if constexpr (N == dynamic)
        {
            return columns;
        }
        else
        {
            return N;
        }
    }

    template<typename T = Type, typename = std::enable_if_t<M == dynamic || M == 1 || N == dynamic || N == 1, T>>
    auto length() const
    {
        if constexpr (M == dynamic)
        {
            if constexpr (N == dynamic)
            {
                if (rows == 1 || columns == 1)
                {
                    return rows * columns;
                }
                else
                {
                    THROW(std::runtime_error, "Matrix is not a vector");
                }
            }
            else if constexpr (N == 1)
            {
                return rows;
            }
            else 
            {
                if (rows == 1)
                {
                    return N;
                }
                else
                {
                    THROW(std::runtime_error, "Matrix is not a vector");
                }
            }
        }
        else if constexpr (N == dynamic)
        {
            if constexpr (M == 1)
            {
                return rows;
            }
            else
            {
                if (columns == 1)
                {
                    return M;
                }
                else
                {
                    THROW(std::runtime_error, "Matrix is not a vector");
                }
            }
        }
        else
        {
            return M * N;
        }
    }
};

}
