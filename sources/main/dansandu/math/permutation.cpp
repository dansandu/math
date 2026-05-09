#include "dansandu/math/permutation.hpp"
#include "dansandu/journey/exception.hpp"

#include <vector>

namespace dansandu::math::permutation
{

std::vector<int> getIdentityPermutation(const int size)
{
    if (size < 0)
    {
        THROW(std::logic_error, "Permutation cannot have negative size ", size);
    }

    auto result = std::vector<int>{};

    result.reserve(size);

    for (auto index = 0; index < size; ++index)
    {
        result.push_back(index);
    }

    return result;
}

std::vector<int> getInvertedPermutation(const std::vector<int>& permutation)
{
    auto inverted = std::vector<int>(permutation.size(), 0);

    for (decltype(permutation.size()) index = 0; index < permutation.size(); ++index)
    {
        inverted[permutation[index]] = static_cast<int>(index);
    }

    return inverted;
}

}
