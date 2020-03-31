#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/predictive_random_generator.hpp"
#include "dansandu/math/clustering.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::ballotin::predictive_random_generator::PredictiveRandomGenerator;
using dansandu::math::clustering::kMeans;
using dansandu::math::matrix::close;
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
        auto clusterCount = 2;
        auto maximumIterations = 10;
        auto precision = 1.0e-4;
        auto expectedCentroids = std::make_pair(Point{{6.66667, -1.75}}, Point{{-5.8, 4.0}});
        auto expectedLabels = std::vector<int>{{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}};
        auto [centroids, labels] =
            kMeans<double, 2, PredictiveRandomGenerator>(samples, clusterCount, maximumIterations);

        REQUIRE(centroids.size() == clusterCount);

        REQUIRE(close(centroids[0], expectedCentroids.first, precision));

        REQUIRE(close(centroids[1], expectedCentroids.second, precision));

        REQUIRE(labels == expectedLabels);
    }
}
