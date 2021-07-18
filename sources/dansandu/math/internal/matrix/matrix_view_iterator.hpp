#pragma once

#include <iterator>

namespace dansandu::math::matrix
{

template<typename T>
class ConstantMatrixViewIterator;

template<typename T>
class MatrixViewIterator
{
    friend ConstantMatrixViewIterator<T>;

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

    MatrixViewIterator(pointer viewBegin, difference_type viewColumnCount, difference_type sourceColumnCount)
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

    auto& operator*() const
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
