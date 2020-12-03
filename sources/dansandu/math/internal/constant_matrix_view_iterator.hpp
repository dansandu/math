#pragma once

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

    ConstantMatrixViewIterator(difference_type sourceColumnCount, difference_type viewColumnCount, pointer viewBegin)
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

    difference_type sourceColumnCount_;
    difference_type viewColumnCount_;
    difference_type viewIndex_;
    pointer viewBegin_;
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
