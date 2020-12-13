#pragma once

#include "dansandu/math/matrix.hpp"

namespace dansandu::math::clustering
{

std::pair<dansandu::math::matrix::Matrix<float>, std::vector<int>>
kMeans(dansandu::math::matrix::ConstantMatrixView<float> samples, int clusters, int iterations);

}
