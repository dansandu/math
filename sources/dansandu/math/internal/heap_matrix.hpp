#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/common_matrix.hpp"

#include <vector>

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N>
class DataStorage<T, M, N, DataStorageStrategy::heap> : private DimensionalityStorage<T, M, N>
{
public:
    DataStorage()
    {
        if constexpr (M != dynamic && N != dynamic && M != 0 && N != 0)
        {
            data_ = std::vector<T>(M * N, dansandu::math::common::additiveIdentity<T>);
        }
    }

    template<size_type L, typename = std::enable_if_t<isVectorOfLength(M, N, L)>>
    explicit DataStorage(const T (&array)[L]) : data_{array, array + L}
    {
        if constexpr (M == dynamic)
        {
            if constexpr (N == L)
            {
                DimensionalityStorage<T, M, N>::rows = 1;
            }
            else
            {
                DimensionalityStorage<T, M, N>::rows = L;
            }
        }
        else
        {
            DimensionalityStorage<T, M, N>::rows = M;
        }

        if constexpr (N == dynamic)
        {
            if constexpr (M == 1)
            {
                DimensionalityStorage<T, M, N>::columns = L;
            }
            else
            {
                DimensionalityStorage<T, M, N>::columns = 1;
            }
        }
        else
        {
            DimensionalityStorage<T, M, N>::columns = N;
        }
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    explicit DataStorage(const T (&array)[MM][NN]) : DimensionalityStorage<T, M, N>{MM, NN}
    {
        data_.reserve(MM * NN);
        for (auto row = 0; row < MM; ++row)
        {
            for (auto column = 0; column < NN; ++column)
            {
                data_.push_back(array[row][column]);
            }
        }
    }

    DataStorage(size_type rows, size_type columns, T fillValue = dansandu::math::common::additiveIdentity<T>)
        : DimensionalityStorage<T, M, N>{rows, columns}
    {
        if ((rows < 0) | (columns < 0) | ((M != dynamic) & (M != rows)) | ((N != dynamic) & (N != columns)))
        {
            THROW(std::out_of_range, "matrix dimensions cannot be negative ", rows, "x", columns,
                  " and must match static rows and columns if not dynamic");
        }
        data_ = std::vector<T>(rowCount() * columnCount(), fillValue);
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

    std::vector<T> data_;
};

}
