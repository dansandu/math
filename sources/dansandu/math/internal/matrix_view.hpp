#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/common_matrix.hpp"
#include "dansandu/math/internal/dimensionality_storage.hpp"
#include "dansandu/math/internal/matrix_view_iterator.hpp"

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N>
class DataStorage<T, M, N, DataStorageStrategy::view> : private DimensionalityStorage<T, M, N>
{
public:
    DataStorage(size_type viewRowCount, size_type viewColumnCount, size_type sourceRowCount,
                size_type sourceColumnCount, T* viewBegin)
        : DimensionalityStorage<T, M, N>{viewRowCount, viewColumnCount},
          viewBegin_{viewBegin},
          sourceRowCount_{sourceRowCount},
          sourceColumnCount_{sourceColumnCount}
    {
    }

    auto& unsafeSubscript(size_type row, size_type column) const
    {
        return viewBegin_[getIndex(row, column)];
    }

    auto& unsafeSubscript(size_type coordinate) const
    {
        return viewBegin_[getIndex(coordinate)];
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
        return sourceRowCount_;
    }

    auto sourceColumnCount() const
    {
        return sourceColumnCount_;
    }

    auto begin() const
    {
        return MatrixViewIterator<T>{viewBegin_, columnCount(), sourceColumnCount_};
    }

    auto end() const
    {
        return MatrixViewIterator<T>{viewBegin_, columnCount(), sourceColumnCount_} + rowCount() * columnCount();
    }

    auto cbegin() const
    {
        return begin();
    }

    auto cend() const
    {
        return end();
    }

    auto data() const
    {
        return viewBegin_;
    }

private:
    auto getIndex(size_type row, size_type column) const
    {
        return row * sourceColumnCount_ + column;
    }

    auto getIndex(size_type index) const
    {
        if (rowCount() == 1)
        {
            return index;
        }
        else
        {
            return index * sourceColumnCount_;
        }
    }

    T* viewBegin_;
    size_type sourceRowCount_;
    size_type sourceColumnCount_;
};

}
