#pragma once

#include "dansandu/math/internal/matrix.hpp"

namespace dansandu::math::matrix
{

template<size_type BeginRow, size_type BeginColumn, size_type Rows, size_type Columns>
class Slicer
{
public:
    template<typename T, size_type M, size_type N, DataStorageStrategy S, typename... A>
    static std::enable_if_t<
        sizeof...(A) == dynamics(BeginRow, BeginColumn, Rows, Columns) && subinterval(BeginRow, Rows, M) &&
            subinterval(BeginColumn, Columns, N),
        std::conditional_t<isView(S), MatrixView<T, Rows, Columns>, ConstantMatrixView<T, Rows, Columns>>>
    slice(const MatrixImplementation<T, M, N, S>& matrix, A... arguments)
    {
        const auto [viewBeginRow, viewBeginColumn, viewRows, viewColumns] = unpackArguments(matrix, arguments...);

        const auto viewBegin = matrix.data() + viewBeginRow * matrix.sourceColumnCount() + viewBeginColumn;

        return {viewRows, viewColumns, matrix.sourceRowCount(), matrix.sourceColumnCount(), viewBegin};
    }

    template<typename T, size_type M, size_type N, DataStorageStrategy S, typename... A>
    static std::enable_if_t<sizeof...(A) == dynamics(BeginRow, BeginColumn, Rows, Columns) && isData(S) &&
                                subinterval(BeginRow, Rows, M) && subinterval(BeginColumn, Columns, N),
                            MatrixView<T, Rows, Columns>>
    slice(MatrixImplementation<T, M, N, S>& matrix, A... arguments)
    {
        const auto [viewBeginRow, viewBeginColumn, viewRows, viewColumns] = unpackArguments(matrix, arguments...);

        const auto viewBegin = matrix.data() + viewBeginRow * matrix.sourceColumnCount() + viewBeginColumn;

        return {viewRows, viewColumns, matrix.sourceRowCount(), matrix.sourceColumnCount(), viewBegin};
    }

private:
    template<typename T, size_type M, size_type N, DataStorageStrategy S, typename... A>
    static auto unpackArguments(const MatrixImplementation<T, M, N, S>& matrix, A... arguments)
    {
        size_type index = 0;
        size_type unpacked[4] = {0};
        (..., (unpacked[index++] = arguments));
        index = 0;

        const auto viewBeginRow = BeginRow != dynamic ? BeginRow : unpacked[index++];
        const auto viewBeginColumn = BeginColumn != dynamic ? BeginColumn : unpacked[index++];

        const auto viewRows = Rows != dynamic ? Rows : unpacked[index++];
        const auto viewColumns = Columns != dynamic ? Columns : unpacked[index++];

        if constexpr (BeginRow == dynamic || Rows == dynamic || M == dynamic)
        {
            if (viewBeginRow < 0 || viewRows < 1 || matrix.rowCount() < viewBeginRow + viewRows)
            {
                THROW(std::out_of_range, "cannot slice rows [", viewBeginRow, ", ", viewBeginRow + viewRows, ") of a ",
                      matrix.rowCount(), "x", matrix.columnCount(), " matrix -- slice exceeds matrix boundries");
            }
        }

        if constexpr (BeginColumn == dynamic || Columns == dynamic || N == dynamic)
        {
            if (viewBeginColumn < 0 || viewColumns < 1 || matrix.columnCount() < viewBeginColumn + viewColumns)
            {
                THROW(std::out_of_range, "cannot slice columns [", viewBeginColumn, ", ", viewBeginColumn + viewColumns,
                      ") of a ", matrix.rowCount(), "x", matrix.columnCount(),
                      " matrix -- slice exceeds matrix boundries");
            }
        }

        return std::make_tuple(viewBeginRow, viewBeginColumn, viewRows, viewColumns);
    }
};

template<typename M>
auto sliceRow(M&& matrix, int row)
{
    constexpr auto staticColumnCount = std::decay_t<M>::staticColumnCount;
    if constexpr (staticColumnCount == dynamic)
    {
        return Slicer<dynamic, 0, 1, staticColumnCount>::slice(std::forward<M>(matrix), row, matrix.columnCount());
    }
    else
    {
        return Slicer<dynamic, 0, 1, staticColumnCount>::slice(std::forward<M>(matrix), row);
    }
}

template<typename M>
auto sliceColumn(M&& matrix, int column)
{
    constexpr auto staticRowCount = std::decay_t<M>::staticRowCount;
    if constexpr (staticRowCount == dynamic)
    {
        return Slicer<0, dynamic, staticRowCount, 1>::slice(std::forward<M>(matrix), column, matrix.rowCount());
    }
    else
    {
        return Slicer<0, dynamic, staticRowCount, 1>::slice(std::forward<M>(matrix), column);
    }
}

}
