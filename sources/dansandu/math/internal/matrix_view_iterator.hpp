#pragma once

#include "dansandu/math/internal/matrix_view_iterator.hpp"

#include <iterator>

namespace dansandu::math::matrix
{

template<typename T>
class MatrixViewIterator
{
    friend bool operator==(MatrixViewIterator left, MatrixViewIterator right)
    {
        return left.getSourcePosition() == right.getSourcePosition();
    }

public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = long long;
    using value_type = T;
    using pointer = value_type*;
    using reference = value_type&;

    MatrixViewIterator(difference_type sourceColumnCount, difference_type viewColumnCount, pointer viewBegin)
        : sourceColumnCount_{sourceColumnCount}, viewColumnCount_{viewColumnCount}, viewIndex_{0}, viewBegin_{viewBegin}
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

    auto& operator*() const
    {
        return *getSourcePosition();
    }

    auto operator->() const
    {
        return getSourcePosition();
    }

    operator ConstantMatrixViewIterator<T>() const
    {
        return {sourceColumnCount_, viewColumnCount_, viewIndex_, viewBegin_};
    }

private:
    auto getSourcePosition() const
    {
        return viewBegin_ + (viewIndex_ / viewColumnCount_) * sourceColumnCount_ + viewIndex_ % viewColumnCount_;
    }

    difference_type sourceColumnCount_;
    difference_type viewColumnCount_;
    difference_type viewIndex_;
    pointer viewBegin_;
};

template<typename T>
auto operator+(MatrixViewIterator<T> iterator, size_type n)
{
    return iterator += n;
}

template<typename T>
auto operator+(size_type n, MatrixViewIterator<T> iterator)
{
    return iterator += n;
}

template<typename T>
auto operator-(MatrixViewIterator<T> iterator, size_type n)
{
    return iterator -= n;
}


template<typename T>
bool operator!=(MatrixViewIterator<T> left, MatrixViewIterator<T> right)
{
    return !(left == right);
}

}
