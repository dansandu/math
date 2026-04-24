#pragma once

#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/logging.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/matrix/common.hpp"
#include "dansandu/math/internal/matrix/dimensionality_storage.hpp"

#include <array>
#include <stacktrace>

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N>
class DataStorage<T, M, N, DataStorageStrategy::stack> : private DimensionalityStorage<T, M, N>
{
public:
    using iterator = typename std::array<T, M * N>::iterator;
    using const_iterator = typename std::array<T, M * N>::const_iterator;

    DataStorage() : DimensionalityStorage<T, M, N>{M, N}, referenceCount_{0}
    {
        std::fill(begin(), end(), additiveIdentity<T>);
    }

    template<size_type L>
        requires(isVectorOfLength(M, N, L))
    explicit DataStorage(const T (&array)[L]) : DimensionalityStorage<T, M, N>{M, N}, referenceCount_{0}
    {
        std::copy(array, array + L, begin());
    }

    template<size_type MM, size_type NN>
        requires(dimensionsMatch(M, N, MM, NN))
    explicit DataStorage(const T (&array)[MM][NN]) : DimensionalityStorage<T, M, N>{MM, NN}, referenceCount_{0}
    {
        for (auto row = 0; row < MM; ++row)
        {
            for (auto column = 0; column < NN; ++column)
            {
                unsafeSubscript(row, column) = array[row][column];
            }
        }
    }

    DataStorage(size_type rows, size_type columns, const T& fillValue)
        : DimensionalityStorage<T, M, N>{rows, columns}, referenceCount_{0}
    {
        if (rows < 0 || columns < 0 || M != rows || N != columns)
        {
            THROW(std::logic_error, "Matrix dimensions do not match: target matrix ", rowCount(), "x", columnCount(),
                  ", source matrix ", rows, "x", columns);
        }
        std::fill(begin(), end(), fillValue);
    }

    template<typename IteratorBegin, typename IteratorEnd>
    DataStorage(size_type rows, size_type columns, IteratorBegin sourceBegin, IteratorEnd sourceEnd)
        : DimensionalityStorage<T, M, N>{rows, columns}, referenceCount_{0}
    {
        if (rows < 0 || columns < 0 || M != rows || N != columns)
        {
            THROW(std::logic_error, "Matrix dimensions do not match: target matrix ", rowCount(), "x", columnCount(),
                  ", source matrix ", rows, "x", columns);
        }
        auto iterator = begin();
        auto sourceIterator = sourceBegin;
        for (auto i = 0; i < rowCount() * columnCount(); ++i)
        {
            if (sourceIterator == sourceEnd)
            {
                THROW(std::out_of_range, "source underflows matrix");
            }
            *iterator++ = *sourceIterator++;
        }
        if (sourceIterator != sourceEnd)
        {
            THROW(std::out_of_range, "source overflows matrix");
        }
    }

    DataStorage(const DataStorage& other)
        : DimensionalityStorage<T, M, N>{other}, data_{other.data_}, referenceCount_{0}
    {
    }

    // The other matrix data is copied so the reference count is set to zero.
    DataStorage(DataStorage&& other) noexcept
        : DimensionalityStorage<T, M, N>{other}, data_{other.data_}, referenceCount_{0}
    {
    }

    ~DataStorage() noexcept
    {
        assert(referenceCount_ == 0 && "No matrix views should reference this matrix container after its life span");
    }

    DataStorage& operator=(const DataStorage& other)
    {
        if (this != &other)
        {
            // Matrix views referencing this matrix are still valid because the dimensions and data address haven't
            // changed.

            data_ = other.data_;
        }

        return *this;
    }

    DataStorage& operator=(DataStorage&& other) noexcept
    {
        if (this != &other)
        {
            // Matrix views referencing this matrix are still valid because the dimensions and data address haven't
            // changed.

            data_ = other.data_;
        }

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

    auto referenceCount() const
    {
        return &referenceCount_;
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
    mutable int referenceCount_;
};

}
