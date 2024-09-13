#include "dansandu/math/clustering.hpp"
#include "dansandu/ballotin/exception.hpp"
#include "dansandu/math/common.hpp"

#include <algorithm>
#include <limits>
#include <random>

using dansandu::math::matrix::ConstantMatrixView;
using dansandu::math::matrix::distance;
using dansandu::math::matrix::dynamic;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::MatrixView;
using dansandu::math::matrix::sliceRow;

namespace dansandu::math::clustering
{

std::vector<int> kMeans(const ConstantMatrixView<float> samples, const MatrixView<float> centroids,
                        const int iterations)
{
    if (centroids.rowCount() <= 0)
    {
        THROW(std::invalid_argument, "invalid cluster count ", centroids.rowCount(),
              " -- centroids row count must be greater than zero");
    }

    if (centroids.columnCount() != samples.columnCount())
    {
        THROW(std::invalid_argument, "centroids column count ", centroids.columnCount(),
              " does not match samples column count ", samples.columnCount());
    }

    auto labels = std::vector<int>(samples.rowCount());
    auto newCentroids = Matrix<float>{centroids.rowCount(), centroids.columnCount()};
    auto count = std::vector<int>(centroids.rowCount());
    for (auto iteration = 0; iteration < iterations; ++iteration)
    {
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
        centroids.deepCopy(newCentroids);
        std::fill(newCentroids.begin(), newCentroids.end(), 0.0f);
        std::fill(count.begin(), count.end(), 0);
    }

    return labels;
}

}
