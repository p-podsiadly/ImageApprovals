#include <ImageApprovals/ImageComparator.hpp>
#include <ImageApprovals/ImageView.hpp>
#include <algorithm>
#include <sstream>

namespace ImageApprovals {

bool ImageComparator::compare(const ImageView& left, const ImageView& right, std::string& outMessage) const
{
    std::string infosMsg, contentsMsg;

    if (!compareInfos(left, right, infosMsg))
    {
         outMessage = "Image are different.";

        if(!infosMsg.empty())
        {
            outMessage += "\n" + infosMsg;
        }

        return false;
    }

    if (!compareContents(left, right, contentsMsg))
    {
        outMessage = "Image contents are different.";

        if (!contentsMsg.empty())
        {
            outMessage += "\n" + contentsMsg;
        }

        return false;
    }

    return true;
}

bool ImageComparator::compareInfos(const ImageView& left, const ImageView& right, std::string& outMessage) const
{
    std::ostringstream msg;

    if (left.getPixelFormat() != right.getPixelFormat())
    {
        msg << "Pixel formats do not match: " << left.getPixelFormat() << " vs " << right.getPixelFormat();
        outMessage = msg.str();
        return false;
    }

    if (left.getColorSpace() != right.getColorSpace())
    {
        msg << "Color spaces do not match: " << left.getColorSpace() << " vs " << right.getColorSpace();
        outMessage = msg.str();
        return false;
    }

    if (left.getSize() != right.getSize())
    {
        msg << "Sizes do not match: " << left.getSize() << " vs " << right.getSize();
        outMessage = msg.str();
        return false;
    }

    return true;
}

ThresholdImageComparator::ThresholdImageComparator(float pixelFailThreshold, float maxFailedPixelsPercentage)
    : m_pixelFailThreshold(pixelFailThreshold), m_maxFailedPixelsPercentage(maxFailedPixelsPercentage)
{}

namespace {

float maxAbsDiff(const RGBA& left, const RGBA& right)
{
    float result = std::abs(left.r - right.r);
    result = std::max(result, std::abs(left.g - right.g));
    result = std::max(result, std::abs(left.b - right.b));
    result = std::max(result, std::abs(left.a - right.a));
    return result;
}

}

bool ThresholdImageComparator::compareContents(const ImageView& left, const ImageView& right, std::string& outMessage) const
{
    const auto sz = left.getSize();

    uint32_t numAboveThreshold = 0;

    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto leftPixel = left.getPixel(x, y);
            const auto rightPixel = right.getPixel(x, y);

            const float diff = maxAbsDiff(leftPixel, rightPixel);
            if (diff > m_pixelFailThreshold)
            {
                ++numAboveThreshold;
            }
        }
    }

    const auto numPixels = static_cast<double>(sz.width) * static_cast<double>(sz.height);
    return (numAboveThreshold / numPixels) <= (m_maxFailedPixelsPercentage / 100.0);
}

}