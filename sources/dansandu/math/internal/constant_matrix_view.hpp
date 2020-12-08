#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/common_matrix.hpp"
#include "dansandu/math/internal/constant_matrix_view_iterator.hpp"

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N>
class DataStorage<T, M, N, DataStorageStrategy::constantView> : private DimensionalityStorage<T, M, N>
{
public:
    DataStorage(size_type viewRowCount, size_type viewColumnCount, size_type sourceRowCount,
                size_type sourceColumnCount, const T* viewBegin)
        : DimensionalityStorage<T, M, N>{viewRowCount, viewColumnCount},
          sourceRowCount_{sourceRowCount},
          sourceColumnCount_{sourceColumnCount},
          viewBegin_{viewBegin}
    {
    }

    const auto& unsafeSubscript(size_type row, size_type column) const
    {
        return viewBegin_[getIndex(row, column)];
    }

    const auto& unsafeSubscript(size_type coordinate) const
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
        return ConstantMatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_};
    }

    auto end() const
    {
        return ConstantMatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_} +
               rowCount() * columnCount();
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
        if (sourceRowCount_ == 1)
        {
            return index;
        }
        else
        {
            return index * sourceColumnCount_;
        }
    }

    size_type sourceRowCount_;
    size_type sourceColumnCount_;
    const T* viewBegin_;
};

}
