#include <ImageApprovals/Comparator.hpp>
#include <ImageApprovals/ImageCodec.hpp>
#include <sstream>
#include <iterator>
#include <stdexcept>

namespace ImageApprovals {

Comparator::Disposer::Disposer(std::vector<ApprovalTests::ComparatorDisposer> disposers)
    : m_disposers(std::move(disposers))
{}

Comparator::Comparator()
    : m_compareStrategy(std::make_shared<ThresholdCompareStrategy>())
{}

Comparator::Comparator(std::shared_ptr<CompareStrategy> comparator)
    : m_compareStrategy(std::move(comparator))
{}

namespace {

Image readImage(const std::string& which, const std::string& path)
{
    try
    {
        return ImageCodec::read(path);
    }
    catch (const std::exception & exc)
    {
        const auto msg =
            "Failed to read " + which + " image from \""
            + path + "\": " + exc.what();

        throw ApprovalTests::ApprovalException(msg);
    }
}

}

bool Comparator::contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const
{
    const Image receivedImg = readImage("received", receivedPath);
    const Image approvedImg = readImage("approved", approvedPath);

    const auto result = m_compareStrategy->compare(approvedImg, receivedImg);
    if (!result.passed)
    {
        throw ApprovalTests::ApprovalMismatchException(result.rightImageInfo, result.leftImageInfo);
    }

    return true;
}

Comparator::Disposer Comparator::registerForAllExtensions(std::shared_ptr<CompareStrategy> strategy)
{
    using namespace ApprovalTests;

    auto comparator = std::make_shared<Comparator>(std::move(strategy));

    const auto allExtensions = ImageCodec::getRegisteredExtensions();
    
    std::vector<ApprovalTests::ComparatorDisposer> disposers;
    disposers.reserve(allExtensions.size());

    transform(allExtensions.begin(), allExtensions.end(), std::back_inserter(disposers),
        [&](const std::string& ext) { return FileApprover::registerComparatorForExtension(ext, comparator); });

    return Disposer(std::move(disposers));
}

}