#include "dansandu/math/clustering.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"
#include "dansandu/range/range.hpp"

#include <random>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::distance;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::sliceRow;

using namespace dansandu::range::range;

namespace dansandu::math::clustering
{

std::pair<Matrix<float>, std::vector<int>> kMeans(ConstantMatrixView<float> samples, int clusters, int iterations)
{
    if (clusters <= 0)
    {
        THROW(std::invalid_argument, "invalid cluster count ", clusters, " -- cluster count must be positive");
    }

    auto centroids = Matrix<float>{clusters, samples.columnCount()};

    auto generator = std::minstd_rand{};
    integers(0, 1, samples.rowCount()) | shuffle(generator) | take(clusters) |
        consume(
            [i = 0, &samples, &centroids](auto j) mutable { sliceRow(centroids, i++).deepCopy(sliceRow(samples, j)); });

    auto labels = std::vector<int>(samples.rowCount());

    for (auto iteration = 0; iteration < iterations; ++iteration)
    {
        auto newCentroids = Matrix<float>{centroids.rowCount(), centroids.columnCount()};
        auto count = std::vector<int>(centroids.rowCount());
        for (auto s = 0; s < samples.rowCount(); ++s)
        {
            auto label = 0;
            auto minimumDistance = -1.0f;
            auto sample = sliceRow(samples, s);
            for (auto c = 0; c < centroids.rowCount(); ++c)
            {
                auto centroid = sliceRow(centroids, c);
                auto d = distance(sample, centroid);
                if (minimumDistance < 0.0f || d < minimumDistance)
                {
                    label = c;
                    minimumDistance = d;
                }
            }
            labels[s] = label;
            sliceRow(newCentroids, label) += sample;
            ++count[label];
        }
        for (auto c = 0; c < centroids.rowCount(); ++c)
        {
            sliceRow(newCentroids, c) /= count[c];
        }
        centroids = std::move(newCentroids);
    }
    return {std::move(centroids), std::move(labels)};
}

}
