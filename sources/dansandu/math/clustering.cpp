#include "dansandu/math/clustering.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/range/range.hpp"

#include <limits>
#include <random>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::distance;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::sliceRow;

using namespace dansandu::range::range;

namespace dansandu::math::clustering
{

static Matrix<float> selectCentroidsAtRandom(const ConstantMatrixView<float> samples, const int clusters)
{
    auto centroids = Matrix<float>{clusters, samples.columnCount()};
    auto generator = std::minstd_rand{};
    integers(0, 1, samples.rowCount()) | shuffle(generator) | take(clusters) |
        forEach(
            [i = 0, &samples, &centroids](auto j) mutable { sliceRow(centroids, i++).deepCopy(sliceRow(samples, j)); });
    return centroids;
}

std::pair<Matrix<float>, std::vector<int>> kMeans(const ConstantMatrixView<float> samples, const int clusters,
                                                  const int iterations)
{
    if (clusters <= 0)
    {
        THROW(std::invalid_argument, "invalid cluster count ", clusters, " -- cluster count must be positive");
    }

    auto centroids = selectCentroidsAtRandom(samples, clusters);
    auto labels = std::vector<int>(samples.rowCount());
    for (auto iteration = 0; iteration < iterations; ++iteration)
    {
        auto newCentroids = Matrix<float>{centroids.rowCount(), centroids.columnCount()};
        auto count = std::vector<int>(centroids.rowCount());
        for (auto s = 0; s < samples.rowCount(); ++s)
        {
            const auto sample = sliceRow(samples, s);
            auto label = 0;
            auto minimumDistance = std::numeric_limits<float>::max();
            for (auto c = 0; c < centroids.rowCount(); ++c)
            {
                const auto centroid = sliceRow(centroids, c);
                const auto d = distance(sample, centroid);
                const auto isCloser = d < minimumDistance;
                label = isCloser * c + !isCloser * label;
                minimumDistance = isCloser * d + !isCloser * minimumDistance;
            }
            labels[s] = label;
            sliceRow(newCentroids, label) += sample;
            ++count[label];
        }
        for (auto c = 0; c < centroids.rowCount(); ++c)
        {
            sliceRow(newCentroids, c) /= static_cast<float>(count[c]);
        }
        centroids = std::move(newCentroids);
    }
    return {std::move(centroids), std::move(labels)};
}

}
