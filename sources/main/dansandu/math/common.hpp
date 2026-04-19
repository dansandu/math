#pragma once

#include <cmath>
#include <type_traits>
#include <utility>

namespace dansandu::math
{

template<typename T>
constexpr T get_pi()
{
    return static_cast<T>(3.141592653589793238462643383279502884197169399375105820974944592307816406286L);
}

template<typename T>
struct NumericTraits
{
};

template<>
struct NumericTraits<int>
{
    static constexpr int additive_identity = 0;
    static constexpr int multiplicative_identity = 1;
    static constexpr int pi = get_pi<int>();
};

template<>
struct NumericTraits<float>
{
    static constexpr float additive_identity = 0.0F;
    static constexpr float multiplicative_identity = 1.0F;
    static constexpr float pi = get_pi<float>();
};

template<>
struct NumericTraits<double>
{
    static constexpr double additive_identity = 0.0;
    static constexpr double multiplicative_identity = 1.0;
    static constexpr double pi = get_pi<double>();
};

template<typename T>
constexpr auto additiveIdentity = NumericTraits<T>::additive_identity;

template<typename T>
constexpr auto multiplicativeIdentity = NumericTraits<T>::multiplicative_identity;

template<typename T>
constexpr auto pi = NumericTraits<T>::pi;

template<typename T, typename U>
constexpr auto close(const T& a, const U& b, const double relative = 1.0e-6, const double absolute = 1.0e-6)
{
    using std::abs;
    return abs(a - b) <= (absolute + relative * abs(b));
}

template<typename T>
class QuadraticEquation
{
public:
    using value_type = std::decay_t<T>;

    QuadraticEquation(value_type a, value_type b, value_type c) : a_{a}, b_{b}, delta_{b * b - 4 * a * c}
    {
    }

    auto hasRealSolutions() const
    {
        return delta_ >= 0;
    }

    auto getRoots() const
    {
        auto deltaSquareRoot = std::sqrt(delta_);
        auto denominator = 2 * a_;
        return std::make_pair((-b_ - deltaSquareRoot) / denominator, (-b_ + deltaSquareRoot) / denominator);
    }

private:
    value_type a_;
    value_type b_;
    value_type delta_;
};

}
