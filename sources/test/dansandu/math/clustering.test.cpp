#include "dansandu/math/clustering.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::math::clustering::kMeans;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::sliceRow;
using dansandu::radiance::Tolerance;

TEST_CASE("clustering")
{
    SECTION("k-means")
    {
        //        *  |
        //    *      |
        //  * *  *   |         *
        // ----------|-------*--
        //           |      * *
        //   *       |    **
        //  *        |

        const auto topLeftCluster = Matrix<double>{{{-4.0, 2.0}, {-7.0, 3.0}, {-3.0, 6.0}, {-7.0, 5.0}, {-8.0, 4.0}}};

        const auto bottomLeftCluster = Matrix<double>{{{-8.0, -9.0}, {-6.0, -6.0}}};

        const auto middleRightCluster =
            Matrix<double>{{{5.0, -4.0}, {6.0, -4.5}, {9.0, -2.0}, {9.0, -2.0}, {7.0, -1.0}, {8.0, 0.0}, {10.0, 1.0}}};

        auto concatanated = std::vector<double>{};
        concatanated.insert(concatanated.end(), topLeftCluster.cbegin(), topLeftCluster.cend());

        concatanated.insert(concatanated.end(), bottomLeftCluster.cbegin(), bottomLeftCluster.cend());

        concatanated.insert(concatanated.end(), middleRightCluster.cbegin(), middleRightCluster.cend());

        const auto samples = Matrix<double>{14, 2, std::move(concatanated)};

        auto centroids = Matrix<double>{{{10.0, 1.0}, {-7.0, 3.0}, {-8.0, 4.0}}};
        const auto iterations = 20;
        const auto labels = kMeans(samples, centroids, iterations);
        const auto expectedLabels = std::vector<int>{{2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0}};

        REQUIRE(expectedLabels == labels);

        const auto expectedCentroids = Matrix<double>{{{7.71429, -1.78571}, {-7.00, -7.50}, {-5.80, 4.00}}};

        REQUIRE(centroids == Tolerance(expectedCentroids));
    }
}
