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

        const auto approvedPath = TEST_FILE("ComparatorTests.Comparator.Using_Approvals__verify_with_PNG.approved.png");
        const auto receivedPath = TEST_FILE("ComparatorTests.Comparator.Using_Approvals__verify_with_PNG.received_ref.png");

        REQUIRE(comparator.contentsAreEquivalent(receivedPath, approvedPath));
    }

    SUBCASE("Differences are not within tolerance")
    {
        std::unique_ptr<ImageComparator> imgComparator;
        imgComparator.reset(new ThresholdImageComparator(AbsThreshold(0.1), Percent(1.2)));

        Comparator comparator(std::move(imgComparator));

        const auto approvedPath = TEST_FILE("ComparatorTests.Comparator.Using_Approvals__verify_with_PNG.approved.png");
        const auto receivedPath = TEST_FILE("ComparatorTests.Comparator.Using_Approvals__verify_with_PNG.received_ref.png");

        REQUIRE_THROWS_AS(
            comparator.contentsAreEquivalent(receivedPath, approvedPath),
            ApprovalMismatchException);
    }

    SUBCASE("Using Approvals::verify with PNG")
    {
        auto subdirDisposer = Approvals::useApprovalsSubdirectory("../data");

        auto comparator = makeImageComparator<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".png", comparator);

        const auto image = ImageCodec::read(TEST_FILE("ComparatorTests.Comparator.Using_Approvals__verify_with_PNG.received_ref.png"));

        Approvals::verify(ImageWriter(image, Format::PNG));
    }

    SUBCASE("Using Approvals::verify with EXR")
    {
        auto subdirDisposer = Approvals::useApprovalsSubdirectory("../data");

        auto comparator = makeImageComparator<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".exr", comparator);

        const auto image = ImageCodec::read(TEST_FILE("ComparatorTests.Comparator.Using_Approvals__verify_with_EXR.received_ref.exr"));

        Approvals::verify(ImageWriter(image, Format::EXR));
    }
}