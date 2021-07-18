#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/matrix/common.hpp"
#include "dansandu/math/internal/matrix/dimensionality_storage.hpp"

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
        if constexpr (N == L)
        {
            DimensionalityStorage<T, M, N>::setRowCount(1);
        }
        else
        {
            DimensionalityStorage<T, M, N>::setRowCount(L);
        }

        if constexpr (M == 1)
        {
            DimensionalityStorage<T, M, N>::setColumnCount(L);
        }
        else
        {
            DimensionalityStorage<T, M, N>::setColumnCount(1);
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

    DataStorage(size_type rows, size_type columns, const T& fillValue) : DimensionalityStorage<T, M, N>{rows, columns}
    {
        if ((rows < 0) | (columns < 0) | ((M != dynamic) & (M != rows)) | ((N != dynamic) & (N != columns)))
        {
            THROW(std::out_of_range, "matrix dimensions cannot be negative ", rows, "x", columns,
                  " and must match static rows and columns if not dynamic");
        }
        data_ = std::vector<T>(rowCount() * columnCount(), fillValue);
    }

    template<typename IteratorBegin, typename IteratorEnd>
    DataStorage(size_type rows, size_type columns, IteratorBegin sourceBegin, IteratorEnd sourceEnd)
        : DimensionalityStorage<T, M, N>{rows, columns}
    {
        if ((rows < 0) | (columns < 0) | ((M != dynamic) & (M != rows)) | ((N != dynamic) & (N != columns)))
        {
            THROW(std::out_of_range, "matrix dimensions cannot be negative ", rows, "x", columns,
                  " and must match static rows and columns if not dynamic");
        }
        data_.reserve(rowCount() * columnCount());
        auto sourceIterator = sourceBegin;
        for (auto i = 0; i < rowCount() * columnCount(); ++i)
        {
            if (sourceIterator == sourceEnd)
            {
                THROW(std::out_of_range, "source underflows matrix");
            }
            data_.push_back(*sourceIterator++);
        }
        if (sourceIterator != sourceEnd)
        {
            THROW(std::out_of_range, "source overflows matrix");
        }
    }

    DataStorage(const DataStorage&) = default;

    DataStorage(DataStorage&& other) noexcept
        : DimensionalityStorage<T, M, N>{std::move(other)}, data_{std::move(other.data_)}
    {
        other.DimensionalityStorage<T, M, N>::setRowCount(0);
        other.DimensionalityStorage<T, M, N>::setColumnCount(0);
    }

    DataStorage& operator=(const DataStorage&) = default;

    DataStorage& operator=(DataStorage&& other) noexcept
    {
        DimensionalityStorage<T, M, N>::operator=(std::move(other));
        other.DimensionalityStorage<T, M, N>::setRowCount(0);
        other.DimensionalityStorage<T, M, N>::setColumnCount(0);
        data_ = std::move(other.data_);
        return *this;
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

    auto sourceRowCount() const
    {
        return DimensionalityStorage<T, M, N>::rowCount();
    }

    auto sourceColumnCount() const
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

    auto cbegin() const
    {
        return data_.cbegin();
    }

    auto cend() const
    {
        return data_.cend();
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
