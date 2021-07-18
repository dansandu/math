#pragma once

namespace dansandu::math::matrix
{

using size_type = int;

constexpr auto dynamic = -1;

enum class DataStorageStrategy
{
    stack,
    heap,
    view,
    constantView
};

template<typename T, size_type M, size_type N, DataStorageStrategy S>
class DataStorage;

constexpr auto isContainer(DataStorageStrategy strategy)
{
    return strategy == DataStorageStrategy::stack || strategy == DataStorageStrategy::heap;
}

constexpr auto isView(DataStorageStrategy strategy)
{
    return strategy == DataStorageStrategy::view;
}

constexpr auto isConstantView(DataStorageStrategy strategy)
{
    return strategy == DataStorageStrategy::constantView;
}

constexpr auto isNullMatrix(size_type m, size_type n)
{
    return m == 0 || n == 0;
}

constexpr auto isVector(size_type m, size_type n)
{
    return m == dynamic || m == 1 || n == dynamic || n == 1;
}

constexpr auto isVectorOfLength(size_type m, size_type n, size_type len)
{
    return ((m == len && n == 1) || (m == 1 && n == len) || (m == dynamic && n == 1) || (m == 1 && n == dynamic) ||
            (m == dynamic && n == len) || (m == len && n == dynamic) || (m == dynamic && n == dynamic));
}

constexpr auto isVectorOfMinimumLength(size_type m, size_type n, size_type len)
{
    return ((m == 1 || m == dynamic) && (n == dynamic || n >= len)) ||
           ((m == dynamic || m >= len) && (n == 1 || n == dynamic));
}

constexpr auto dimensionsMatch(size_type m, size_type n, size_type mm, size_type nn)
{
    return (m == mm || m == dynamic || mm == dynamic) && (n == nn || n == dynamic || nn == dynamic);
}

constexpr auto strictDimensionsMatch(size_type m, size_type n, size_type mm, size_type nn)
{
    return m == mm && n == nn;
}

constexpr auto canSubscript(size_type m, size_type n, size_type row, size_type column)
{
    return (0 <= row) & (row < m) & (0 <= column) & (column < n);
}

constexpr auto canSubscript(size_type m, size_type n, size_type index)
{
    return (0 <= index) & (((m == 1) & (index < n)) | ((index < m) & (n == 1)));
}

constexpr auto vectorsOfLength3(size_type m, size_type n, size_type mm, size_type nn)
{
    return (((m == 1 || m == dynamic) && (n == 3 || n == dynamic)) ||
            ((m == 3 || m == dynamic) && (n == 1 || n == dynamic))) &&
           (((mm == 1 || mm == dynamic) && (nn == 3 || nn == dynamic)) ||
            ((mm == 3 || mm == dynamic) && (nn == 1 || nn == dynamic)));
}

constexpr auto vectorsOfEqualLength(size_type m, size_type n, size_type mm, size_type nn)
{
    return ((m == 1 || m == dynamic) && (mm == 1 || mm == dynamic) && (n == nn || n == dynamic || nn == dynamic)) ||
           ((m == 1 || m == dynamic) && (nn == 1 || nn == dynamic) && (n == mm || n == dynamic || nn == dynamic)) ||
           ((n == 1 || n == dynamic) && (mm == 1 || mm == dynamic) && (m == nn || m == dynamic || nn == dynamic)) ||
           ((n == 1 || n == dynamic) && (nn == 1 || nn == dynamic) && (m == mm || m == dynamic || nn == dynamic));
}

constexpr auto dynamics(size_type m, size_type n, size_type o, size_type p)
{
    return (m == dynamic) + (n == dynamic) + (o == dynamic) + (p == dynamic);
}

constexpr auto subinterval(size_type a, size_type b, size_type l)
{
    return (a == dynamic || 0 <= a) && (b == dynamic || 1 <= b) && (l == dynamic || a == dynamic || a <= l) &&
           (l == dynamic || b == dynamic || b <= l) && (l == dynamic || a == dynamic || b == dynamic || a + b <= l);
}

}
