#include "dansandu/math/clustering.hpp"
#include "dansandu/math/matrix.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::math::clustering::kMeans;
using dansandu::math::matrix::close;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::sliceRow;

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

        const auto topLeftCluster =
            Matrix<float>{{{-4.0f, 2.0f}, {-7.0f, 3.0f}, {-3.0f, 6.0}, {-7.0f, 5.0}, {-8.0f, 4.0}}};

        const auto bottomLeftCluster = Matrix<float>{{{-8.0f, -9.0f}, {-6.0f, -6.0f}}};

        const auto middleRightCluster = Matrix<float>{
            {{5.0f, -4.0f}, {6.0f, -4.5f}, {9.0f, -2.0f}, {9.0f, -2.0f}, {7.0f, -1.0f}, {8.0f, 0.0f}, {10.0f, 1.0f}}};

        auto concatanated = std::vector<float>{};
        concatanated.insert(concatanated.end(), topLeftCluster.cbegin(), topLeftCluster.cend());

        concatanated.insert(concatanated.end(), bottomLeftCluster.cbegin(), bottomLeftCluster.cend());

        concatanated.insert(concatanated.end(), middleRightCluster.cbegin(), middleRightCluster.cend());

        const auto samples = Matrix<float>{14, 2, std::move(concatanated)};

        auto centroids = Matrix<float>{{{10.0f, 1.0f}, {-7.0f, 3.0f}, {-8.0f, 4.0f}}};
        const auto iterations = 20;
        const auto labels = kMeans(samples, centroids, iterations);
        const auto expectedLabels = std::vector<int>{{2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0}};

        REQUIRE(expectedLabels == labels);

        const auto expectedCentroids = Matrix<float>{{{7.71429f, -1.78571f}, {-7.00f, -7.50f}, {-5.80f, 4.00f}}};
        const auto epsilon = 1.0e-5f;

        REQUIRE(close(expectedCentroids, centroids, epsilon));
    }
}
