#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/common_matrix.hpp"

#include <array>

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N>
class DataStorage<T, M, N, DataStorageStrategy::stack> : private DimensionalityStorage<T, M, N>
{
public:
    DataStorage()
    {
        std::fill(begin(), end(), dansandu::math::common::additiveIdentity<T>);
    }

    template<size_type L, typename = std::enable_if_t<isVectorOfLength(M, N, L)>>
    explicit DataStorage(const T (&array)[L]) : DimensionalityStorage<T, M, N>{M, N}
    {
        std::copy(array, array + L, begin());
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    explicit DataStorage(const T (&array)[MM][NN]) : DimensionalityStorage<T, M, N>{MM, NN}
    {
        for (auto row = 0; row < MM; ++row)
        {
            for (auto column = 0; column < NN; ++column)
            {
                unsafeSubscript(row, column) = array[row][column];
            }
        }
    }

    DataStorage(size_type rows, size_type columns, T fillValue = dansandu::math::common::additiveIdentity<T>)
        : DimensionalityStorage<T, M, N>{rows, columns}
    {
        if ((rows < 0) | (columns < 0) | (M != rows) | (N != columns))
        {
            THROW(std::out_of_range, "matrix dimensions cannot be negative ", rows, "x", columns,
                  " and must match static rows and columns if not dynamic");
        }
        std::fill(begin(), end(), fillValue);
    }

    auto& unsafeSubscript(size_type row, size_type column)
    {
        return data_[getIndex(row, column)];
    }

    const auto& unsafeSubscript(size_type row, size_type column) const
    {
        return data_[getIndex(row, column)];
    }

    auto& unsafeSubscript(size_type coordinate)
    {
        return data_[getIndex(coordinate)];
    }

    const auto& unsafeSubscript(size_type coordinate) const
    {
        return data_[getIndex(coordinate)];
    }

    auto rowCount() const
    {
        return DimensionalityStorage<T, M, N>::rowCount();
    }

    auto columnCount() const
    {
        return DimensionalityStorage<T, M, N>::columnCount();
    }

    auto begin()
    {
        return data_.begin();
    }

    auto end()
    {
        return data_.end();
    }

    auto begin() const
    {
        return data_.begin();
    }

    auto end() const
    {
        return data_.end();
    }

    auto data()
    {
        return data_.data();
    }

    auto data() const
    {
        return data_.data();
    }

private:
    auto getIndex(size_type row, size_type column) const
    {
        return row * columnCount() + column;
    }

    auto getIndex(size_type index) const
    {
        return index;
    }

    std::array<T, M * N> data_;
};

}
