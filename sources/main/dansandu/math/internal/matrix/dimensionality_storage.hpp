#pragma once

#include "dansandu/math/internal/matrix/common.hpp"

namespace dansandu::math::matrix
{

template<typename T>
constexpr auto getDataStorageStrategy(const size_type m, const size_type n)
{
    constexpr auto stackBytesThreshold = static_cast<size_type>(32);

    return (m == dynamic || n == dynamic || m * n * sizeof(T) > stackBytesThreshold) ? DataStorageStrategy::heap
                                                                                     : DataStorageStrategy::stack;
}

template<typename T, size_type M, size_type N>
class DimensionalityStorage;

template<typename T, size_type M, size_type N>
class DimensionalityStorage
{
public:
    DimensionalityStorage()
    {
    }

    DimensionalityStorage(size_type, size_type)
    {
    }

    void setRowCount(size_type)
    {
    }

    void setColumnCount(size_type)
    {
    }

    auto rowCount() const
    {
        return M;
    }

    auto columnCount() const
    {
        return N;
    }
};

template<typename T, size_type N>
class DimensionalityStorage<T, dynamic, N>
{
public:
    DimensionalityStorage() : rows_{0}
    {
    }

    DimensionalityStorage(size_type rows, size_type) : rows_{rows}
    {
    }

    void setRowCount(size_type rows)
    {
        rows_ = rows;
    }

    void setColumnCount(size_type)
    {
    }

    auto rowCount() const
    {
        return rows_;
    }

    auto columnCount() const
    {
        return N;
    }

private:
    size_type rows_;
};

template<typename T, size_type M>
class DimensionalityStorage<T, M, dynamic>
{
public:
    DimensionalityStorage() : columns_{0}
    {
    }

    DimensionalityStorage(size_type, size_type columns) : columns_{columns}
    {
    }

    void setRowCount(size_type)
    {
    }

    void setColumnCount(size_type columns)
    {
        columns_ = columns;
    }

    auto rowCount() const
    {
        return M;
    }

    auto columnCount() const
    {
        return columns_;
    }

private:
    size_type columns_;
};

template<typename T>
class DimensionalityStorage<T, dynamic, dynamic>
{
public:
    DimensionalityStorage() : rows_{0}, columns_{0}
    {
    }

    DimensionalityStorage(size_type rows, size_type columns) : rows_{rows}, columns_{columns}
    {
    }

    void setRowCount(size_type rows)
    {
        rows_ = rows;
    }

    void setColumnCount(size_type columns)
    {
        columns_ = columns;
    }

    auto rowCount() const
    {
        return rows_;
    }

    auto columnCount() const
    {
        return columns_;
    }

private:
    size_type rows_;
    size_type columns_;
};

}
