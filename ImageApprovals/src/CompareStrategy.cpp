#include <ImageApprovals/CompareStrategy.hpp>
#include <ImageApprovals/ImageView.hpp>
#include <cstring>
#define NOMINMAX
#include <ApprovalTests.hpp>
#include <algorithm>

namespace ImageApprovals {

using ApprovalTests::StringUtils;

CompareStrategy::Result CompareStrategy::Result::makePassed()
{
    Result res;
    res.passed = true;
    return res;
}

CompareStrategy::Result CompareStrategy::Result::makeFailed(std::string leftInfo, std::string rightInfo)
{
    Result res;
    res.passed = false;
    res.leftImageInfo = std::move(leftInfo);
    res.rightImageInfo = std::move(rightInfo);
    return res;
}

CompareStrategy::Result CompareStrategy::compare(const ImageView& left, const ImageView& right) const
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

CompareStrategy::Result CompareStrategy::compareInfos(const ImageView& left, const ImageView& right) const
{
    Result result;
    result.passed = false;

    if (left.getPixelFormat() != right.getPixelFormat())
    {
        return Result::makeFailed(
            "pixel format = " + StringUtils::toString(left.getPixelFormat()),
            "pixel format = " + StringUtils::toString(right.getPixelFormat()));
    }

    if (left.getColorSpace() != right.getColorSpace())
    {
        return Result::makeFailed(
            "color space = " + StringUtils::toString(left.getColorSpace()),
            "color space = " + StringUtils::toString(right.getColorSpace()));
    }

    if (left.getSize() != right.getSize())
    {
        return Result::makeFailed(
            "size = " + StringUtils::toString(left.getSize()),
            "size = " + StringUtils::toString(right.getSize()));
    }

    return Result::makePassed();
}

ThresholdCompareStrategy::ThresholdCompareStrategy(AbsThreshold pixelFailThreshold, Percent maxFailedPixelsPercentage)
    : m_pixelFailThreshold(pixelFailThreshold), m_maxFailedPixelsPercentage(maxFailedPixelsPercentage)
{}

namespace detail {

float maxAbsDiff(const RGBA& left, const RGBA& right)
{
    float result = std::abs(left.r - right.r);
    result = std::max(result, std::abs(left.g - right.g));
    result = std::max(result, std::abs(left.b - right.b));
    result = std::max(result, std::abs(left.a - right.a));
    return result;
}

}

CompareStrategy::Result ThresholdCompareStrategy::compareContents(const ImageView& left, const ImageView& right) const
{
    const auto sz = left.getSize();

    uint32_t numAboveThreshold = 0;

    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto leftPixel = left.getPixel(x, y);
            const auto rightPixel = right.getPixel(x, y);

            const float diff = detail::maxAbsDiff(leftPixel, rightPixel);
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
            = StringUtils::toString(numAboveThreshold) + " pixels (" + StringUtils::toString(percentAboveThreshold)
            + ") are above threshold = " + StringUtils::toString(m_pixelFailThreshold);

        return Result::makeFailed("reference image", rightInfo);
    }

    return Result::makePassed();
}

CompareStrategy::Result PixelPerfectCompareStrategy::compareContents(const ImageView& left, const ImageView& right) const
{
    const auto sz = left.getSize();
    const auto rowLen = left.getPixelFormat().getPixelStride() * sz.width;

    for(uint32_t y = 0; y < sz.height; ++y)
    {
        const auto leftRow = left.getRowPointer(y);
        const auto rightRow = right.getRowPointer(y);

        if(0 != std::memcmp(leftRow, rightRow, rowLen))
        {
            return Result::makeFailed("reference image", "different pixels in row " + std::to_string(y));
        }
    }

    return Result::makePassed();
}

}