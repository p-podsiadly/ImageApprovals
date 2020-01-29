#include <ImageApprovals/Comparator.hpp>
#include <ImageApprovals/ImageCodec.hpp>
#include <sstream>
#include <stdexcept>

namespace ImageApprovals {

bool Comparator::contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const
{
    const Image receivedImg = ImageCodec::read(receivedPath);
    const Image approvedImg = ImageCodec::read(approvedPath);

    if (receivedImg.getPixelFormat() != approvedImg.getPixelFormat())
    {
        std::ostringstream msg;
        msg << "Pixel formats are different: ";
        msg << receivedImg.getPixelFormat() << " (received) and ";
        msg << approvedImg.getPixelFormat() << " (approved)";
        throw std::runtime_error(msg.str());
    }

    if (receivedImg.getSize() != approvedImg.getSize())
    {
        std::ostringstream msg;
        msg << "Image sizes are different: ";
        msg << receivedImg.getSize() << " (received) and";
        msg << approvedImg.getSize() << " (approved)";
        throw std::runtime_error(msg.str());
    }

    // TODO
    if (std::memcmp(receivedImg.getPixelData(), approvedImg.getPixelData(), receivedImg.getRowStride() * receivedImg.getSize().height) != 0)
    {
        throw std::runtime_error("Contents of images are not equal");
    }

    return true;
}

}