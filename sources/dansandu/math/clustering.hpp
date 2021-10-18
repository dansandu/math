#pragma once

#include "dansandu/math/matrix.hpp"

#include <vector>

namespace dansandu::math::clustering
{

PRALINE_EXPORT std::vector<int> kMeans(const dansandu::math::matrix::ConstantMatrixView<float> samples,
                                       const dansandu::math::matrix::MatrixView<float> centroids, const int iterations);

}
