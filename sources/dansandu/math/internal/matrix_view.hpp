#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/internal/common_matrix.hpp"
#include "dansandu/math/internal/constant_matrix_view.hpp"
#include "dansandu/math/internal/constant_matrix_view_iterator.hpp"
#include "dansandu/math/internal/matrix_view_iterator.hpp"

namespace dansandu::math::matrix
{

template<typename T = float, size_type M = dynamic, size_type N = dynamic>
class MatrixView : private DimensionalityStorage<T, M, N>
{
public:
    MatrixView(size_type rowCount, size_type columnCount, size_type sourceRowCount, size_type sourceColumnCount,
               T* viewBegin)
        : DimensionalityStorage<T, M, N>{rowCount, columnCount},
          sourceRowCount_{sourceRowCount},
          sourceColumnCount_{sourceColumnCount},
          viewBegin_{viewBegin}
    {
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    auto& operator+=(ConstantMatrixView<T, MM, NN> other) const
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::logic_error, "cannot add matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::transform(cbegin(), cend(), other.cbegin(), begin(), dansandu::math::common::Add{});
        return *this;
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    auto& operator-=(ConstantMatrixView<T, MM, NN> other) const
    {
        if constexpr (M == dynamic || N == dynamic || MM == dynamic || NN == dynamic)
        {
            if ((rowCount() != other.rowCount()) | (columnCount() != other.columnCount()))
            {
                THROW(std::logic_error, "cannot subtract matrices ", rowCount(), "x", columnCount(), " and ",
                      other.rowCount(), "x", other.columnCount(), " -- matrix dimensions do not match");
            }
        }
        std::transform(cbegin(), cend(), other.cbegin(), begin(), dansandu::math::common::Subtract{});
        return *this;
    }

    auto& operator*=(T scalar) const
    {
        std::transform(cbegin(), cend(), begin(), dansandu::math::common::MultiplyBy<T>{scalar});
        return *this;
    }

    template<typename TT = T, typename = std::enable_if_t<!isNullMatrix(M, N), TT>>
    auto& operator()(size_type row, size_type column) const
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
    auto& operator()(size_type coordinate) const
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
    auto& x() const
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
    auto& y() const
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
    auto& z() const
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
    auto& w() const
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

    auto begin() const
    {
        return MatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_};
    }

    auto end() const
    {
        return MatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_} + (rowCount() * columnCount());
    }

    auto cbegin() const
    {
        return ConstantMatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_};
    }

    auto cend() const
    {
        return ConstantMatrixViewIterator<T>{sourceColumnCount_, columnCount(), viewBegin_} +
               (rowCount() * columnCount());
    }

    template<size_type MM, size_type NN, typename = std::enable_if_t<dimensionsMatch(M, N, MM, NN)>>
    operator ConstantMatrixView<T, MM, NN>() const
    {
        return {rowCount(), columnCount(), sourceRowCount_, sourceColumnCount_, viewBegin_};
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
    T* viewBegin_;
};

}
