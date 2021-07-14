#pragma once

#include "dansandu/math/matrix.hpp"

#include <vector>

namespace dansandu::math::clustering
{

PRALINE_EXPORT std::pair<dansandu::math::matrix::Matrix<float>, std::vector<int>>
kMeans(const dansandu::math::matrix::ConstantMatrixView<float> samples, const int clusters, const int iterations);

PRALINE_EXPORT std::pair<dansandu::math::matrix::Matrix<float>, std::vector<int>>
testKmeans(const dansandu::math::matrix::ConstantMatrixView<float> samples, const int clusters, const int iterations);

}
