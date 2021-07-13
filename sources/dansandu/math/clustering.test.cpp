#include "catchorg/catch/catch.hpp"
#include "dansandu/ballotin/test/random.hpp"
#include "dansandu/math/clustering.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::ballotin::test::random::PredictableBitGenerator;
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
        //  * *  *   |    *
        // ----------|-------*--
        //           |      * *
        //   *       |    * *
        //  *        |
        const auto samples = Matrix<float>{{{5.0f, 1.0f},
                                            {-8.0f, 4.0f},
                                            {7.0f, -1.0f},
                                            {-7.0f, 5.0f},
                                            {-7.0f, 3.0f},
                                            {-8.0f, -9.0f},
                                            {-4.0f, 2.0f},
                                            {-3.0f, 6.0f},
                                            {8.0f, 0.0f},
                                            {6.0f, -4.5f},
                                            {-6.0f, -6.0f},
                                            {9.0f, -2.0f},
                                            {5.0f, -4.0f}}};

        auto generator = PredictableBitGenerator{};
        const auto clusters = 3;
        const auto iterations = 10;
        const auto [centroids, labels] = kMeans(samples, clusters, iterations, generator);
        const auto expectedLabels = std::vector<int>{{1, 2, 1, 2, 2, 0, 2, 2, 1, 1, 0, 1, 1}};

        REQUIRE(centroids.rowCount() == clusters);

        REQUIRE(centroids.columnCount() == samples.columnCount());

        REQUIRE(expectedLabels == labels);
    }
}
