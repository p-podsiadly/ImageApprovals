#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;
using namespace ApprovalTests;

TEST_CASE("Comparator")
{
    SUBCASE("Differences are within tolerance")
    {
        auto imgComparator
            = std::make_shared<ThresholdImageComparator>(
                AbsThreshold(0.1), Percent(1.25));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("cornell.approved.png");
        const auto receivedPath = TEST_FILE("cornell.received.png");

        REQUIRE(comparator.contentsAreEquivalent(receivedPath, approvedPath));
    }

    SUBCASE("Differences are not within tolerance")
    {
        auto imgComparator
            = std::make_shared<ThresholdImageComparator>(
                AbsThreshold(0.1), Percent(1.2));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("cornell.approved.png");
        const auto receivedPath = TEST_FILE("cornell.received.png");

        REQUIRE_THROWS_AS(
            comparator.contentsAreEquivalent(receivedPath, approvedPath),
            ApprovalMismatchException);
    }

    SUBCASE("Using FileApprover::verify with PNG")
    {
        auto comparator = Comparator::make<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".png", comparator);

        FileApprover::verify(TEST_FILE("cornell.received.png"), TEST_FILE("cornell.approved.png"));
    }

    SUBCASE("Using FileApprover::verify with EXR")
    {
        auto comparator = Comparator::make<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".exr", comparator);

        FileApprover::verify(TEST_FILE("cornell.received.exr"), TEST_FILE("cornell.approved.exr"));
    }
}

TEST_CASE("Comparator::registerForAllExtensions")
{
    auto comparatorDisposer
        = Comparator::registerForAllExtensions<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));

    FileApprover::verify(TEST_FILE("cornell.received.png"), TEST_FILE("cornell.approved.png"));

    FileApprover::verify(TEST_FILE("cornell.received.exr"), TEST_FILE("cornell.approved.exr"));
}