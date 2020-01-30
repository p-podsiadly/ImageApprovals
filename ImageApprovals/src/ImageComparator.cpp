#include <ImageApprovals/ImageComparator.hpp>
#include <ImageApprovals/StringUtils.hpp>
#include <ImageApprovals/ImageView.hpp>
#include <algorithm>
#include <sstream>

namespace ImageApprovals {

bool ImageComparator::compare(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const
{
    CmpMessage infosMsg, contentsMsg;

    if (!compareInfos(left, right, infosMsg))
    {
        outMessage = infosMsg;
        return false;
    }

    if (!compareContents(left, right, contentsMsg))
    {
        outMessage = contentsMsg;
        return false;
    }

    return true;
}

bool ImageComparator::compareInfos(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const
{
    if (left.getPixelFormat() != right.getPixelFormat())
    {
        outMessage.left = "pixel format = " + toString(left.getPixelFormat());
        outMessage.right = "pixel format = " + toString(right.getPixelFormat());
        return false;
    }

    if (left.getColorSpace() != right.getColorSpace())
    {
        outMessage.left = "color space = " + toString(left.getColorSpace());
        outMessage.right = "color space = " + toString(right.getColorSpace());
        return false;
    }

    if (left.getSize() != right.getSize())
    {
        outMessage.left = "size = " + toString(left.getSize());
        outMessage.right = "size = " + toString(right.getSize());
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

bool ThresholdImageComparator::compareContents(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const
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

    const auto numPixels = static_cast<double>(sz.width)* static_cast<double>(sz.height);
    if ((numAboveThreshold / numPixels) > (m_maxFailedPixelsPercentage / 100.0))
    {
        outMessage.left = "reference image";
        outMessage.right = toString(numPixels) + " are above threshold = " + toString(m_pixelFailThreshold);
        return false;
    }

    return true;
}

}