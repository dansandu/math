#pragma once

#include "dansandu/ballotin/default_random_generator.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/range/range.hpp"

namespace dansandu::math::clustering
{

template<typename T, int N,
         typename RandomGenerator = dansandu::ballotin::default_random_generator::DefaultRandomGenerator>
std::pair<std::vector<dansandu::math::matrix::Matrix<T, 1, N>>, std::vector<int>>
    kMeans(std::vector<dansandu::math::matrix::Matrix<T, 1, N>> samples, int clusterCount, int maximumIterations)
{
    if (clusterCount <= 0)
        THROW(std::invalid_argument, "invalid cluster count ", clusterCount, " -- cluster count must be positive");

    using namespace dansandu::range::range;
    auto centroids = integers(0, 1, samples.size()) | shuffle<RandomGenerator>() | take(clusterCount) |
                     map([&samples](auto i) { return samples[i]; }) | toVector();
    auto labels = std::vector<int>(samples.size());
    auto newCentroids = centroids;
    for (auto iteration = 0; iteration < maximumIterations; ++iteration)
    {
        centroids = newCentroids;
        newCentroids = decltype(centroids)(centroids.size());
        auto count = std::vector<int>(centroids.size());
        for (auto sample = 0U; sample < samples.size(); ++sample)
        {
            auto bestCentroid = 0;
            auto bestDistance = -dansandu::math::common::multiplicative_identity<T>;
            for (auto centroid = 0U; centroid < centroids.size(); ++centroid)
            {
                auto candidateDistance = dansandu::math::matrix::distance(centroids[centroid], samples[sample]);
                if (bestDistance < 0 || candidateDistance < bestDistance)
                {
                    bestDistance = candidateDistance;
                    bestCentroid = centroid;
                }
            }
            labels[sample] = bestCentroid;
            newCentroids[bestCentroid] += samples[sample];
            ++count[bestCentroid];
        }
        for (auto i = 0U; i < newCentroids.size(); ++i)
            newCentroids[i] /= static_cast<T>(count[i]);
    }
    return {std::move(centroids), std::move(labels)};
}

}
