#pragma once

#include <cmath>
#include <type_traits>
#include <utility>

namespace dansandu::math::common {

using size_type = int;

constexpr auto dynamic = -1;

struct add {
    template<typename A, typename B>
    constexpr auto operator()(A&& a, B&& b) const {
        return std::forward<A>(a) + std::forward<B>(b);
    }
};

struct subtract {
    template<typename A, typename B>
    constexpr auto operator()(A&& a, B&& b) const {
        return std::forward<A>(a) - std::forward<B>(b);
    }
};

struct multiply {
    template<typename A, typename B>
    constexpr auto operator()(A&& a, B&& b) const {
        return std::forward<A>(a) * std::forward<B>(b);
    }
};

struct divide {
    template<typename A, typename B>
    constexpr auto operator()(A&& a, B&& b) const {
        return std::forward<A>(a) / std::forward<B>(b);
    }
};

template<typename Scalar>
struct multiplyBy {
    multiplyBy(Scalar scalar) : scalar_{std::move(scalar)} {}

    template<typename Element>
    constexpr auto operator()(Element&& element) const {
        return std::forward<Element>(element) * scalar_;
    }

private:
    Scalar scalar_;
};

template<typename Scalar>
struct divideBy {
    divideBy(Scalar scalar) : scalar_{std::move(scalar)} {}

    template<typename Element>
    constexpr auto operator()(Element&& element) const {
        return std::forward<Element>(element) / scalar_;
    }

private:
    Scalar scalar_;
};

template<typename T>
constexpr T get_pi() {
    return static_cast<T>(3.141592653589793238462643383279502884197169399375105820974944592307816406286L);
}

template<typename T>
struct numeric_traits {};

template<>
struct numeric_traits<int> {
    static constexpr int additive_identity = 0;
    static constexpr int multiplicative_identity = 1;
    static constexpr int pi = get_pi<int>();
};

template<>
struct numeric_traits<float> {
    static constexpr float additive_identity = 0.0F;
    static constexpr float multiplicative_identity = 1.0F;
    static constexpr float pi = get_pi<float>();
};

template<>
struct numeric_traits<double> {
    static constexpr double additive_identity = 0.0;
    static constexpr double multiplicative_identity = 1.0;
    static constexpr double pi = get_pi<double>();
};

template<typename T>
constexpr auto additive_identity = numeric_traits<T>::additive_identity;

template<typename T>
constexpr auto multiplicative_identity = numeric_traits<T>::multiplicative_identity;

template<typename T>
constexpr auto pi = numeric_traits<T>::pi;

template<typename Type>
class QuadraticEquation {
public:
    using value_type = std::decay_t<Type>;

    QuadraticEquation(value_type a, value_type b, value_type c) : a_{a}, b_{b}, delta_{b * b - 4 * a * c} {}

    auto hasRealSolutions() const { return delta_ >= 0; }

    auto getRoots() const {
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
