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

    CmpMessage message;
    if (!m_comparator->compare(approvedImg.getView(), receivedImg.getView(), message))
    {
        throw ApprovalTests::ApprovalMismatchException(message.right, message.left);
    }

    return true;
}

}