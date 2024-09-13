#include "dansandu/math/permutation.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/range/range.hpp"

#include <vector>

using dansandu::range::range::integers;
using dansandu::range::range::toVector;
using dansandu::range::range::operator|;

namespace dansandu::math::permutation
{

std::vector<int> getIdentityPermutation(const int n)
{
    if (n >= 0)
    {
        return integers(0, 1, n) | toVector();
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
