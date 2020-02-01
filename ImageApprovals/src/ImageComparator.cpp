#include <ImageApprovals/ImageComparator.hpp>
#include <ImageApprovals/StringUtils.hpp>
#include <ImageApprovals/ImageView.hpp>
#include <algorithm>
#include <sstream>

namespace ImageApprovals {

ImageComparator::Result ImageComparator::Result::makePassed()
{
    Result res;
    res.passed = true;
    return res;
}

ImageComparator::Result ImageComparator::Result::makeFailed(std::string leftInfo, std::string rightInfo)
{
    Result res;
    res.passed = false;
    res.leftImageInfo = std::move(leftInfo);
    res.rightImageInfo = std::move(rightInfo);
    return res;
}

ImageComparator::Result ImageComparator::compare(const ImageView& left, const ImageView& right) const
{
    Result result;

    if (!(result = compareInfos(left, right)).passed)
    {
        return result;
    }

    if (!(result = compareContents(left, right)).passed)
    {
        return result;
    }

    return Result::makePassed();
}

ImageComparator::Result ImageComparator::compareInfos(const ImageView& left, const ImageView& right) const
{
    Result result;
    result.passed = false;

    if (left.getPixelFormat() != right.getPixelFormat())
    {
        return Result::makeFailed(
            "pixel format = " + toString(left.getPixelFormat()),
            "pixel format = " + toString(right.getPixelFormat()));
    }

    if (left.getColorSpace() != right.getColorSpace())
    {
        return Result::makeFailed(
            "color space = " + toString(left.getColorSpace()),
            "color space = " + toString(right.getColorSpace()));
    }

    if (left.getSize() != right.getSize())
    {
        return Result::makeFailed(
            "size = " + toString(left.getSize()),
            "size = " + toString(right.getSize()));
    }

    return Result::makePassed();
}

ThresholdImageComparator::ThresholdImageComparator(AbsThreshold pixelFailThreshold, Percent maxFailedPixelsPercentage)
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

ImageComparator::Result ThresholdImageComparator::compareContents(const ImageView& left, const ImageView& right) const
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
            if (diff > m_pixelFailThreshold.value)
            {
                ++numAboveThreshold;
            }
        }
    }

    const double numPixels = static_cast<double>(sz.width)* static_cast<double>(sz.height);
    const auto percentAboveThreshold = Percent((numAboveThreshold / numPixels) * 100.0);

    if (percentAboveThreshold > m_maxFailedPixelsPercentage)
    {
        std::string rightInfo
            = toString(numAboveThreshold) + " pixels (" + toString(percentAboveThreshold)
            + ") are above threshold = " + toString(m_pixelFailThreshold);

        return Result::makeFailed("reference image", rightInfo);
    }

    return Result::makePassed();
}

}