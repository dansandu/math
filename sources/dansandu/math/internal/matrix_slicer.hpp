#pragma once

#include "dansandu/math/internal/matrix.hpp"

namespace dansandu::math::matrix
{

template<size_type BeginRow, size_type Rows, size_type BeginColumn, size_type Columns>
class Slicer
{
public:
    template<typename T, size_type M, size_type N, DataStorageStrategy S, typename... A>
    static std::enable_if_t<sizeof...(A) == dynamics(BeginRow, Rows, BeginColumn, Columns) &&
                                subinterval(BeginRow, Rows, M) && subinterval(BeginColumn, Columns, N),
                            ConstantMatrixView<T, Rows, Columns>>
    slice(const MatrixImplementation<T, M, N, S>& matrix, A... arguments)
    {
        size_type index = 0;
        size_type unpacked[4] = {0};
        (..., (unpacked[index++] = arguments));
        index = 0;
        const auto viewBeginRow = BeginRow != dynamic ? BeginRow : unpacked[index++];
        const auto viewRows = Rows != dynamic ? Rows : unpacked[index++];
        const auto viewBeginColumn = BeginColumn != dynamic ? BeginColumn : unpacked[index++];
        const auto viewColumns = Columns != dynamic ? Columns : unpacked[index++];
        const auto viewBegin = matrix.data() + viewBeginRow * matrix.columnCount() + viewBeginColumn;

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

        return {viewRows, viewColumns, matrix.rowCount(), matrix.columnCount(), viewBegin};
    }
};

}
