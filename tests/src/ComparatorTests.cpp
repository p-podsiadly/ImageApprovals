#include <doctest/doctest.h>
#include <ImageApprovals/Comparator.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;

TEST_CASE("Comparator")
{
    SUBCASE("Differences are within tolerance")
    {
        std::unique_ptr<ImageComparator> imgComparator;
        imgComparator.reset(new ThresholdImageComparator(AbsThreshold(0.1), Percent(1.25)));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("cornell_approved.png");
        const auto receivedPath = TEST_FILE("cornell_received.png");

        REQUIRE(comparator.contentsAreEquivalent(receivedPath, approvedPath));
    }

    SUBCASE("Differences are not within tolerance")
    {
        std::unique_ptr<ImageComparator> imgComparator;
        imgComparator.reset(new ThresholdImageComparator(AbsThreshold(0.1), Percent(1.2)));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("cornell_approved.png");
        const auto receivedPath = TEST_FILE("cornell_received.png");

        REQUIRE_THROWS_AS(
            comparator.contentsAreEquivalent(receivedPath, approvedPath),
            ApprovalTests::ApprovalMismatchException);
    }
}