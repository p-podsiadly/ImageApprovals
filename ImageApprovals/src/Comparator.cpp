#include <ImageApprovals/Comparator.hpp>
#include <ImageApprovals/ImageCodec.hpp>
#include <sstream>
#include <stdexcept>

namespace ImageApprovals {

Comparator::Comparator()
    : m_comparator(new ThresholdImageComparator())
{}

Comparator::Comparator(std::unique_ptr<ImageComparator> comparator)
    : m_comparator(std::move(comparator))
{}

bool Comparator::contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const
{
    const Image receivedImg = ImageCodec::read(receivedPath);
    const Image approvedImg = ImageCodec::read(approvedPath);

    const auto result = m_comparator->compare(approvedImg.getView(), receivedImg.getView());
    if (!result.passed)
    {
        throw ApprovalTests::ApprovalMismatchException(result.rightImageInfo, result.leftImageInfo);
    }

    return true;
}

Comparator::Disposer Comparator::registerForAllExtensionsImpl(std::unique_ptr<ImageComparator> imageComparator)
{
    using namespace ApprovalTests;

    auto comparator = std::make_shared<Comparator>(std::move(imageComparator));

    const auto allExtensions = ImageCodec::getRegisteredExtensions();
    
    std::vector<ApprovalTests::ComparatorDisposer> disposers;
    disposers.reserve(allExtensions.size());

    transform(allExtensions.begin(), allExtensions.end(), std::back_inserter(disposers),
        [&](const std::string& ext) { return FileApprover::registerComparatorForExtension(ext, comparator); });

    return Disposer(std::move(disposers));
}

Comparator::Disposer::Disposer(std::vector<ApprovalTests::ComparatorDisposer> disposers)
    : m_disposers(std::move(disposers))
{}

}