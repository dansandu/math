#include "catchorg/catch/catch.hpp"
#include "dansandu/math/clustering.hpp"
#include "dansandu/math/matrix.hpp"

using dansandu::math::clustering::kMeans;
using dansandu::math::matrix::close;
using dansandu::math::matrix::Matrix;
using dansandu::math::matrix::sliceRow;

// clang-format off
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
        const auto samples = Matrix<float>{{{-3.0f,  6.0f},
                                            {-7.0f,  5.0f},
                                            {-8.0f,  4.0f},
                                            {-7.0f,  3.0f},
                                            {-4.0f,  2.0f},
                                            { 5.0f,  1.0f},
                                            { 8.0f,  0.0f},
                                            { 7.0f, -1.0f},
                                            { 9.0f, -2.0f},
                                            { 5.0f, -4.0f},
                                            { 6.0f, -4.5f}}};

        const auto clusters = 2;
        const auto iterations = 10;
        const auto epsilon = 1.0e-5f;
        const auto [centroids, labels] = kMeans(samples, clusters, iterations);

        REQUIRE(centroids.rowCount() == clusters);

        REQUIRE(centroids.columnCount() == samples.columnCount());

        REQUIRE(static_cast<int>(labels.size()) == samples.rowCount());

        REQUIRE(std::all_of(labels.cbegin(), labels.cend(), [](auto l) { return l == 0 || l == 1; }));

        REQUIRE(!close(sliceRow(centroids, 0), sliceRow(centroids, 1), epsilon));
    }
}
// clang-format on
