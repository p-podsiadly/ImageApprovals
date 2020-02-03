#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;
using namespace ApprovalTests;

TEST_CASE("Comparator")
{
    SUBCASE("Differences are within tolerance")
    {
        std::unique_ptr<ImageComparator> imgComparator;
        imgComparator.reset(new ThresholdImageComparator(AbsThreshold(0.1), Percent(1.25)));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("cornell.approved.png");
        const auto receivedPath = TEST_FILE("cornell.received_ref.png");

        REQUIRE(comparator.contentsAreEquivalent(receivedPath, approvedPath));
    }

    SUBCASE("Differences are not within tolerance")
    {
        std::unique_ptr<ImageComparator> imgComparator;
        imgComparator.reset(new ThresholdImageComparator(AbsThreshold(0.1), Percent(1.2)));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("cornell.approved.png");
        const auto receivedPath = TEST_FILE("cornell.received_ref.png");

        REQUIRE_THROWS_AS(
            comparator.contentsAreEquivalent(receivedPath, approvedPath),
            ApprovalMismatchException);
    }

    SUBCASE("Using FileApprover::verify with PNG")
    {
        auto comparator = Comparator::make<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".png", comparator);

        FileApprover::verify(TEST_FILE("cornell.received_ref.png"), TEST_FILE("cornell.approved.png"));
    }

    SUBCASE("Using FileApprover::verify with EXR")
    {
        auto comparator = Comparator::make<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".exr", comparator);

        FileApprover::verify(TEST_FILE("cornell.received_ref.exr"), TEST_FILE("cornell.approved.exr"));
    }
}

// TODO: remove should_fail once this is resolved: https://github.com/approvals/ApprovalTests.cpp/pull/93
TEST_CASE("Comparator::registerForAllExtensions" * doctest::should_fail())
{
    auto comparatorDisposer
        = Comparator::registerForAllExtensions<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));

    FileApprover::verify(TEST_FILE("cornell.received_ref.png"), TEST_FILE("cornell.approved.png"));

    FileApprover::verify(TEST_FILE("cornell.received_ref.exr"), TEST_FILE("cornell.approved.exr"));
}