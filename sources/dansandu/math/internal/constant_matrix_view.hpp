#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/internal/common_matrix.hpp"
#include "dansandu/math/internal/constant_matrix_view_iterator.hpp"
#include "dansandu/math/internal/matrix_view_iterator.hpp"

namespace dansandu::math::matrix
{

template<typename T = float, size_type M = dynamic, size_type N = dynamic>
class ConstantMatrixView : private DimensionalityStorage<T, M, N>
{
public:
    ConstantMatrixView(size_type rowCount, size_type columnCount, size_type sourceRowCount, size_type sourceColumnCount,
                       const T* viewBegin)
        : DimensionalityStorage<T, M, N>{rowCount, columnCount},
          sourceRowCount_{sourceRowCount},
          sourceColumnCount_{sourceColumnCount},
          viewBegin_{viewBegin}
    {
    }

    template<typename TT = T, typename = std::enable_if_t<!isNullMatrix(M, N), TT>>
    const auto& operator()(size_type row, size_type column) const
    {
        if (canSubscript(rowCount(), columnCount(), row, column))
        {
            return unsafeSubscript(row, column);
        }
        else
        {
            THROW(std::out_of_range, "cannot index the (", row, ", ", column, ") element in a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVector(M, N), TT>>
    const auto& operator()(size_type coordinate) const
    {
        if (canSubscript(rowCount(), columnCount(), coordinate))
        {
            return unsafeSubscript(coordinate);
        }
        else
        {
            THROW(std::out_of_range, "cannot index the ", coordinate, "th element of a ", rowCount(), "x",
                  columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 1), TT>>
    const auto& x() const
    {
        if (canSubscript(rowCount(), columnCount(), 0))
        {
            return unsafeSubscript(0);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the x coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 2), TT>>
    const auto& y() const
    {
        if (canSubscript(rowCount(), columnCount(), 1))
        {
            return unsafeSubscript(1);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the y coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 3), TT>>
    const auto& z() const
    {
        if (canSubscript(rowCount(), columnCount(), 2))
        {
            return unsafeSubscript(2);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the z coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
    }

    template<typename TT = T, typename = std::enable_if_t<isVectorOfMinimumLength(M, N, 4), TT>>
    const auto& w() const
    {
        if (canSubscript(rowCount(), columnCount(), 3))
        {
            return unsafeSubscript(3);
        }
        else
        {
            THROW(std::out_of_range, "cannot get the w coordinate of a ", rowCount(), "x", columnCount(), " matrix");
        }
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

    auto begin() const
    {
        return ConstantMatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_};
    }

    auto end() const
    {
        return ConstantMatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_} +
               (rowCount() * columnCount());
    }

    auto cbegin() const
    {
        return begin();
    }

    auto cend() const
    {
        return end();
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
