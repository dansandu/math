#pragma once

#include <vector>

namespace dansandu::math::permutation
{

PRALINE_EXPORT std::vector<int> getIdentityPermutation(const int n);

PRALINE_EXPORT std::vector<int> getInvertedPermutation(const std::vector<int>& permutation);

}
