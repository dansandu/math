#include "catchorg/catch/catch.hpp"
#include "dansandu/math/clustering.hpp"
#include "dansandu/math/matrix.hpp"

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
        auto precision = 1.0e-10;
        auto [c, l] = kMeans(samples, clusterCount, maximumIterations);
        auto centroids = std::move(c);
        auto labels = std::move(l);

        REQUIRE(centroids.size() == clusterCount);

        REQUIRE(!close(centroids.front(), centroids.back(), precision));

        REQUIRE(std::any_of(samples.cbegin(), samples.cend(),
                            [&centroids, precision](const auto& s) { return close(s, centroids.front(), precision); }));

        REQUIRE(std::any_of(samples.cbegin(), samples.cend(),
                            [&centroids, precision](const auto& s) { return close(s, centroids.back(), precision); }));

        REQUIRE(labels.size() == samples.size());

        REQUIRE(std::all_of(labels.cbegin(), labels.cend(), [](auto l) { return l == 0 || l == 1; }));
    }
}
