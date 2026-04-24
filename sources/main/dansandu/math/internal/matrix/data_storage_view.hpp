#pragma once

#include "dansandu/journey/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/internal/matrix/common.hpp"
#include "dansandu/math/internal/matrix/dimensionality_storage.hpp"
#include "dansandu/math/internal/matrix/matrix_view_iterator.hpp"

namespace dansandu::math::matrix
{

template<typename T, size_type M, size_type N>
class DataStorage<T, M, N, DataStorageStrategy::view> : private DimensionalityStorage<T, M, N>
{
public:
    using iterator = MatrixViewIterator<T>;
    using const_iterator = ConstantMatrixViewIterator<T>;

    DataStorage(size_type viewRowCount, size_type viewColumnCount, size_type sourceRowCount,
                size_type sourceColumnCount, T* viewBegin, int* const referenceCount)
        : DimensionalityStorage<T, M, N>{viewRowCount, viewColumnCount},
          viewBegin_{viewBegin},
          sourceRowCount_{sourceRowCount},
          sourceColumnCount_{sourceColumnCount},
          referenceCount_{referenceCount}
    {
        ++*referenceCount_;
    }

    DataStorage(const DataStorage& other)
        : DimensionalityStorage<T, M, N>{other.rowCount(), other.columnCount()},
          viewBegin_{other.viewBegin_},
          sourceRowCount_{other.sourceRowCount_},
          sourceColumnCount_{other.sourceColumnCount_},
          referenceCount_{other.referenceCount_}
    {
        ++*referenceCount_;
    }

    DataStorage(DataStorage&& other) noexcept
        : DimensionalityStorage<T, M, N>{other.rowCount(), other.columnCount()},
          viewBegin_{other.viewBegin_},
          sourceRowCount_{other.sourceRowCount_},
          sourceColumnCount_{other.sourceColumnCount_},
          referenceCount_{other.referenceCount_}
    {
        ++*referenceCount_;
    }

    ~DataStorage() noexcept
    {
        --*referenceCount_;
    }

    DataStorage& operator=(const DataStorage& other)
    {
        --*referenceCount_;

        DimensionalityStorage<T, M, N>::operator=(other);

        viewBegin_ = other.viewBegin_;
        sourceRowCount_ = other.sourceRowCount_;
        sourceColumnCount_ = other.sourceColumnCount_;
        referenceCount_ = other.referenceCount_;

        ++*referenceCount_;

        return *this;
    }

    DataStorage& operator=(DataStorage&& other) noexcept
    {
        --*referenceCount_;

        DimensionalityStorage<T, M, N>::operator=(other);

        viewBegin_ = other.viewBegin_;
        sourceRowCount_ = other.sourceRowCount_;
        sourceColumnCount_ = other.sourceColumnCount_;
        referenceCount_ = other.referenceCount_;

        ++*referenceCount_;

        return *this;
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
        return ConstantMatrixViewIterator<T>{viewBegin_, columnCount(), sourceColumnCount_};
    }

    auto cend() const
    {
        return ConstantMatrixViewIterator<T>{viewBegin_, columnCount(), sourceColumnCount_} +
               rowCount() * columnCount();
    }

    auto data() const
    {
        return viewBegin_;
    }

    auto referenceCount() const
    {
        return referenceCount_;
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
    int* referenceCount_;
};

}
