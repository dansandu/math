#include "dansandu/math/permutation.hpp"
#include "dansandu/ballotin/exception.hpp"

#include <ranges>
#include <vector>

namespace dansandu::math::permutation
{

std::vector<int> getIdentityPermutation(const int n)
{
    if (n >= 0)
    {
        return std::views::iota(0, n) | std::ranges::to<std::vector>();
    }
    else
    {
        THROW(std::logic_error, "permutation cannot be negative value ", n);
    }
}

std::vector<int> getInvertedPermutation(const std::vector<int>& permutation)
{
    auto inverted = std::vector<int>(permutation.size(), 0);

    for (decltype(permutation.size()) index = 0; index < permutation.size(); ++index)
    {
        inverted[permutation[index]] = index;
    }

    return inverted;
}

}
