#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;
using namespace ApprovalTests;

namespace {

class FixedNameNamer : public ApprovalTestNamer
{
public:
    explicit FixedNameNamer(std::string baseName)
        : m_baseName(std::move(baseName))
    {}

    std::string getApprovedFile(std::string extensionWithDot) const override
    {
        return getDirectory() + m_baseName + ".approved" + extensionWithDot;
    }

    std::string getReceivedFile(std::string extensionWithDot) const override
    {
        return getDirectory() + m_baseName + ".received" + extensionWithDot;
    }

private:
    std::string m_baseName;
};

}

TEST_CASE("Comparator")
{
    DefaultNamerFactory::setDefaultNamer([]() { return std::make_shared<FixedNameNamer>("cornell"); });

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

    SUBCASE("Using Approvals::verify with PNG")
    {
        auto subdirDisposer = Approvals::useApprovalsSubdirectory("../data");

        auto comparator = Comparator::make<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".png", comparator);

        const auto image = ImageCodec::read(TEST_FILE("cornell.received_ref.png"));

        Approvals::verify(ImageWriter(image));
    }

    SUBCASE("Using Approvals::verify with EXR")
    {
        auto subdirDisposer = Approvals::useApprovalsSubdirectory("../data");

        auto comparator = Comparator::make<ThresholdImageComparator>(AbsThreshold(0.1), Percent(1.25));
        auto comparatorDisposer = FileApprover::registerComparatorForExtension(".exr", comparator);

        const auto image = ImageCodec::read(TEST_FILE("cornell.received_ref.exr"));

        Approvals::verify(ImageWriter(image));
    }
}