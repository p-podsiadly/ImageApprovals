#include <doctest/doctest.h>
#include <ImageApprovals/ImageComparator.hpp>
#include <ImageApprovals/Image.hpp>

using namespace ImageApprovals;

TEST_CASE("ThresholdImageComparator")
{
    ThresholdImageComparator comparator(0.004f, 5.0f);

    const PixelFormat format{ PixelLayout::RGB, PixelDataType::UInt8 };
    const auto& colorSpace = ColorSpace::getLinear();
    const Size size{ 20, 1 };

    Image left(format, colorSpace, size);

    std::string msg;

    SUBCASE("Differences are within thresholds")
    {
        Image right(format, colorSpace, size);
        auto pixels = right.getPixelData();
        pixels[6] = 2;

        REQUIRE(comparator.compare(left.getView(), right.getView(), msg));
    }

    SUBCASE("Differences are not within thresholds")
    {
        Image right(format, colorSpace, size);
        auto pixels = right.getPixelData();
        pixels[6] = 2;
        pixels[10] = 2;
        pixels[14] = 2;
        
        REQUIRE_FALSE(comparator.compare(left.getView(), right.getView(), msg));
    }
}