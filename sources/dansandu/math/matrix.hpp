#pragma once

#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"

#include <algorithm>
#include <array>
#include <ostream>
#include <vector>

namespace dansandu::math::matrix
{

using dansandu::math::common::size_type;

enum class StorageStrategy
{
    stack,
    dynamic
};

template<typename Type, size_type M, size_type N, StorageStrategy storageStrategy>
struct Storage;

template<typename Type, size_type M, size_type N>
struct Storage<Type, M, N, StorageStrategy::stack>
{
    std::array<Type, M * N> data;
};

template<typename Type, size_type M, size_type N>
struct Storage<Type, M, N, StorageStrategy::dynamic>
{
    std::vector<Type> data;
};

template<typename Type, size_type M, size_type N>
class Matrix
{
public:
    static_assert(M >= 0, "matrix row count must be positive");
    static_assert(N >= 0, "matrix column count must be positive");
    static_assert((M == 0) == (N == 0), "zero rows implies zero columns and vice-versa");

    using value_type = std::decay_t<Type>;
    using size_type = dansandu::math::common::size_type;

    template<typename T = value_type, typename = std::enable_if_t<std::is_integral_v<T>>>
    friend auto operator==(const Matrix& a, const Matrix& b)
    {
        return std::equal(std::begin(a.storage_.data), std::end(a.storage_.data), std::begin(b.storage_.data));
    }

private:
    static constexpr auto storageStrategy = M * N * sizeof(value_type) > 4 * sizeof(int) ? StorageStrategy::dynamic
                                                                                         : StorageStrategy::stack;

    static constexpr auto vectorOfMinimumLength(size_type n) noexcept
    {
        return (M == 1 && N >= n) || (M >= n && N == 1);
    }

    auto uncheckedIndex(size_type row, size_type column) const noexcept
    {
        return row * columnCount() + column;
    }

    auto checkedIndex(size_type row, size_type column) const
    {
        if ((row >= 0) & (row < rowCount()) & (column >= 0) & (column < columnCount()))
            return uncheckedIndex(row, column);
        THROW(std::out_of_range, "cannot index the (", row, ", ", column, ") element in a ", rowCount(), "x",
              columnCount(), " matrix");
    }

    auto checkedIndex(size_type index) const
    {
        if ((index >= 0) & (index < length()))
            return index;
        THROW(std::out_of_range, "cannot index the ", index, "th element in a ", rowCount(), "x", columnCount(),
              " matrix");
    }

public:
    constexpr Matrix() : Matrix{dansandu::math::common::additive_identity<value_type>}
    {
    }

    explicit constexpr Matrix(value_type fillValue)
    {
        if constexpr (storageStrategy == StorageStrategy::stack)
            std::fill(std::begin(storage_.data), std::end(storage_.data), fillValue);
        else
            storage_.data = std::vector<value_type>(M * N, fillValue);
    }

    template<typename... Arguments,
             typename = std::enable_if_t<(M == 1 || N == 1) && M * N >= 2 && sizeof...(Arguments) == M * N &&
                                         (std::is_convertible_v<std::decay_t<Arguments>, value_type> && ...)>>
    explicit constexpr Matrix(Arguments&&... arguments)
    {
        if constexpr (storageStrategy == StorageStrategy::stack)
        {
            auto begin = storage_.data.begin();
            (..., (*begin++ = std::forward<Arguments>(arguments)));
        }
        else
        {
            storage_.data.reserve(sizeof...(Arguments));
            (..., (storage_.data.push_back(std::forward<Arguments>(arguments))));
        }
    }

    template<size_type length, typename T = value_type,
             typename = std::enable_if_t<(M == length && N == 1) || (M == 1 && N == length), T>>
    explicit constexpr Matrix(const value_type (&array)[length])
    {
        if constexpr (storageStrategy == StorageStrategy::stack)
            std::copy(std::begin(array), std::end(array), std::begin(storage_.data));
        else
            storage_.data = std::vector<value_type>{std::begin(array), std::end(array)};
    }

    template<size_type m, size_type n, typename T = value_type, typename = std::enable_if_t<M == m && N == n, T>>
    explicit constexpr Matrix(const value_type (&array)[m][n])
    {
        if constexpr (storageStrategy == StorageStrategy::dynamic)
        {
            storage_.data.reserve(M * N);
        }
        for (auto row = 0; row < M; ++row)
            for (auto column = 0; column < N; ++column)
                if constexpr (storageStrategy == StorageStrategy::stack)
                    storage_.data[uncheckedIndex(row, column)] = array[row][column];
                else
                    storage_.data.push_back(array[row][column]);
    }

    Matrix(const Matrix& other) = default;

    Matrix(Matrix&& other) = default;

    Matrix& operator=(const Matrix& other) = default;

    Matrix& operator=(Matrix&& other) = default;

    auto& operator+=(const Matrix& other)
    {
        std::transform(std::begin(storage_.data), std::end(storage_.data), std::begin(other.storage_.data),
                       std::begin(storage_.data), dansandu::math::common::add{});
        return *this;
    }

    auto& operator-=(const Matrix& other)
    {
        std::transform(std::begin(storage_.data), std::end(storage_.data), std::begin(other.storage_.data),
                       std::begin(storage_.data), dansandu::math::common::subtract{});
        return *this;
    }

    auto& operator*=(const value_type& scalar)
    {
        std::transform(std::begin(storage_.data), std::end(storage_.data), std::begin(storage_.data),
                       dansandu::math::common::multiplyBy<value_type>{scalar});
        return *this;
    }

    auto& operator/=(const value_type& scalar)
    {
        std::transform(std::begin(storage_.data), std::end(storage_.data), std::begin(storage_.data),
                       dansandu::math::common::divideBy<value_type>{scalar});
        return *this;
    }

    auto& operator()(size_type row, size_type column)
    {
        return storage_.data[checkedIndex(row, column)];
    }

    const auto& operator()(size_type row, size_type column) const
    {
        return storage_.data[checkedIndex(row, column)];
    }

    template<typename T = value_type, typename = std::enable_if_t<M == 1 || N == 1, T>>
    auto& operator()(size_type index)
    {
        return storage_.data[checkedIndex(index)];
    }

    template<typename T = value_type, typename = std::enable_if_t<M == 1 || N == 1, T>>
    const auto& operator()(size_type index) const
    {
        return storage_.data[checkedIndex(index)];
    }

    template<typename T = value_type, typename = std::enable_if_t<Matrix::vectorOfMinimumLength(1), T>>
    const auto& x() const
    {
        return storage_.data[0];
    }

    template<typename T = value_type, typename = std::enable_if_t<vectorOfMinimumLength(2), T>>
    const auto& y() const
    {
        return storage_.data[1];
    }

    template<typename T = value_type, typename = std::enable_if_t<vectorOfMinimumLength(3), T>>
    const auto& z() const
    {
        return storage_.data[2];
    }

    template<typename T = value_type, typename = std::enable_if_t<vectorOfMinimumLength(4), T>>
    const auto& w() const
    {
        return storage_.data[3];
    }

    auto rowCount() const noexcept
    {
        return M;
    }

    auto columnCount() const noexcept
    {
        return N;
    }

    template<typename T = value_type, typename = std::enable_if_t<M == 1 || N == 1, T>>
    auto length() const
    {
        return M * N;
    }

    auto begin() const
    {
        return storage_.data.cbegin();
    }

    auto end() const
    {
        return storage_.data.cend();
    }

    auto cbegin() const
    {
        return storage_.data.cbegin();
    }

    auto cend() const
    {
        return storage_.data.cend();
    }

private:
    Storage<value_type, M, N, storageStrategy> storage_;
};

template<typename Type, size_type M, size_type N>
auto operator+(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    auto copy = a;
    return copy += b;
}

template<typename Type, size_type M, size_type N>
auto operator-(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    auto copy = a;
    return copy -= b;
}

template<typename Type, size_type M, size_type N>
auto operator-(const Matrix<Type, M, N>& matrix)
{
    return -dansandu::math::common::multiplicative_identity<Type> * matrix;
}

template<typename Type, size_type M, size_type N>
auto operator*(const Matrix<Type, M, N>& matrix, const typename Matrix<Type, M, N>::value_type& scalar)
{
    auto copy = matrix;
    return copy *= scalar;
}

template<typename Type, size_type M, size_type N>
auto operator*(const typename Matrix<Type, M, N>::value_type& scalar, const Matrix<Type, M, N>& matrix)
{
    auto copy = matrix;
    return copy *= scalar;
}

template<typename Type, size_type M, size_type N>
auto operator/(const Matrix<Type, M, N>& matrix, const typename Matrix<Type, M, N>::value_type& scalar)
{
    auto copy = matrix;
    return copy /= scalar;
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<M == 1 || N == 1, Type>>
auto magnitude(const Matrix<Type, M, N>& matrix)
{
    return std::sqrt(dotProduct(matrix, matrix));
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<M == 1 || N == 1, Type>>
auto normalized(const Matrix<Type, M, N>& matrix)
{
    return matrix / magnitude(matrix);
}

template<typename Type, size_type M, size_type N, size_type P>
auto operator*(const Matrix<Type, M, N>& a, const Matrix<Type, N, P>& b)
{
    auto result =
        Matrix<Type, M, P>{dansandu::math::common::additive_identity<typename Matrix<Type, M, P>::value_type>};
    for (auto m = 0; m < M; ++m)
        for (auto p = 0; p < P; ++p)
            for (auto n = 0; n < N; ++n)
                result(m, p) += a(m, n) * b(n, p);
    return result;
}

template<typename Type, size_type M, size_type N>
auto close(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b,
           const typename Matrix<Type, M, N>::value_type& epsilon)
{
    for (auto left = a.cbegin(), right = b.cbegin(); left != a.cend(); ++left, ++right)
        if (*left - *right > epsilon)
            return false;
    return true;
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<M == 1 || N == 1, Type>>
auto dotProduct(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    auto result = dansandu::math::common::additive_identity<typename Matrix<Type, M, N>::value_type>;
    for (auto left = a.cbegin(), right = b.cbegin(); left != a.cend(); ++left, ++right)
        result += (*left) * (*right);
    return result;
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<(M == 1 || N == 1) && M * N == 3, Type>>
auto crossProduct(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    return Matrix<Type, M, N>{
        {a.y() * b.z() - b.y() * a.z(), b.x() * a.z() - a.x() * b.z(), a.x() * b.y() - b.x() * a.y()}};
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<M == 1 || N == 1, Type>>
auto distance(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    return magnitude(a - b);
}

template<typename Type, size_type M, size_type N, typename = std::enable_if_t<std::is_integral_v<Type>>>
auto operator!=(const Matrix<Type, M, N>& a, const Matrix<Type, M, N>& b)
{
    return !(a == b);
}

template<typename Type, size_type M, size_type N>
std::ostream& operator<<(std::ostream& stream, const Matrix<Type, M, N>& matrix)
{
    stream << "{";
    auto firstRow = true;
    for (auto row = 0; row < matrix.rowCount(); ++row)
    {
        if (!firstRow)
            stream << ", ";
        stream << "{";
        auto firstElement = true;
        for (auto column = 0; column < matrix.columnCount(); ++column)
        {
            if (!firstElement)
                stream << ", ";
            firstElement = false;
            stream << matrix(row, column);
        }
        firstRow = false;
        stream << "}";
    }
    return stream << "}";
}
}
