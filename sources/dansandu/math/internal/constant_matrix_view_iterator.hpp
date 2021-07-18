#pragma once

#include "dansandu/math/internal/matrix_view_iterator.hpp"

#include <iterator>

namespace dansandu::math::matrix
{

template<typename T>
class ConstantMatrixViewIterator
{
    friend bool operator==(ConstantMatrixViewIterator left, ConstantMatrixViewIterator right)
    {
        return left.getSourcePosition() == right.getSourcePosition();
    }

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = long long;
    using value_type = T;
    using pointer = const value_type*;
    using reference = const value_type&;

    ConstantMatrixViewIterator(const MatrixViewIterator<T>& iterator)
        : viewBegin_{iterator.viewBegin_},
          viewIndex_{iterator.viewIndex_},
          viewColumnCount_{iterator.viewColumnCount_},
          sourceColumnCount_{iterator.sourceColumnCount_}
    {
    }

    ConstantMatrixViewIterator(pointer viewBegin, difference_type viewColumnCount, difference_type sourceColumnCount)
        : viewBegin_{viewBegin}, viewIndex_{0}, viewColumnCount_{viewColumnCount}, sourceColumnCount_{sourceColumnCount}
    {
    }

    auto& operator+=(size_type n)
    {
        viewIndex_ += n;
        return *this;
    }

    auto& operator-=(size_type n)
    {
        viewIndex_ -= n;
        return *this;
    }

    auto& operator++()
    {
        viewIndex_ += 1;
        return *this;
    }

    auto operator++(int)
    {
        auto copy = *this;
        ++*this;
        return copy;
    }

    auto& operator--()
    {
        viewIndex_ -= 1;
        return *this;
    }

    auto operator--(int)
    {
        auto copy = *this;
        --*this;
        return copy;
    }

    const auto& operator*() const
    {
        return *getSourcePosition();
    }

    auto operator->() const
    {
        return getSourcePosition();
    }

private:
    auto getSourcePosition() const
    {
        return viewBegin_ + (viewIndex_ / viewColumnCount_) * sourceColumnCount_ + viewIndex_ % viewColumnCount_;
    }

    pointer viewBegin_;
    difference_type viewIndex_;
    difference_type viewColumnCount_;
    difference_type sourceColumnCount_;
};

template<typename T>
auto operator+(ConstantMatrixViewIterator<T> iterator, size_type n)
{
    return iterator += n;
}

template<typename T>
auto operator+(size_type n, ConstantMatrixViewIterator<T> iterator)
{
    return iterator += n;
}

template<typename T>
auto operator-(ConstantMatrixViewIterator<T> iterator, size_type n)
{
    return iterator -= n;
}

template<typename T>
bool operator!=(ConstantMatrixViewIterator<T> left, ConstantMatrixViewIterator<T> right)
{
    return !(left == right);
}

}
