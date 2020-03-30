#include "catchorg/catch/catch.hpp"
#include "dansandu/math/clustering.hpp"

using dansandu::math::clustering::kMeans;
using Point = dansandu::math::matrix::Matrix<double, 1, 2>;

TEST_CASE("Clustering")
{
    SECTION("k-means")
    {
        //        *  |
        //    *      |
        //  * *  *   |    *
        // ----------|-------*--
        //           |      * *
        //           |    * *
        //           |
        auto samples =
            std::vector<Point>{{Point{{-3.0, 6.0}}, Point{{-7.0, 5.0}}, Point{{-8.0, 4.0}}, Point{{-7.0, 3.0}},
                                Point{{-4.0, 2.0}}, Point{{5.0, 1.0}}, Point{{8.0, 0.0}}, Point{{7.0, -1.0}},
                                Point{{9.0, -2.0}}, Point{{5.0, -4.0}}, Point{{6.0, -4.5}}}};

        auto maximumIterations = 15;
        auto clusterCount = 2;
        auto [centroids, labels] = kMeans(samples, clusterCount, maximumIterations);

        REQUIRE(centroids.size() == clusterCount);

        REQUIRE(labels.size() == samples.size());
    }
}
