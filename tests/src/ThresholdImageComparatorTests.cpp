#include <doctest/doctest.h>
#include <ImageApprovals.hpp>

using namespace ImageApprovals;

TEST_CASE("ThresholdImageComparator")
{
    ThresholdImageComparator comparator(AbsThreshold(0.004), Percent(5.0));

    const PixelFormat& format = PixelFormat::getRgbU8();
    const auto& colorSpace = ColorSpace::getLinearSRgb();
    const Size size{ 20, 1 };

    Image left(format, colorSpace, size);
    
    SUBCASE("Differences are within thresholds")
    {
        Image right(format, colorSpace, size);
        auto pixels = right.getPixelData();
        pixels[6] = 2;

        REQUIRE(comparator.compare(left, right).passed);
    }

    SUBCASE("Differences are not within thresholds")
    {
        Image right(format, colorSpace, size);
        auto pixels = right.getPixelData();
        pixels[6] = 2;
        pixels[10] = 2;
        pixels[14] = 2;
        
        REQUIRE_FALSE(comparator.compare(left, right).passed);
    }

    SUBCASE("Images have different pixel formats")
    {
        Image right(PixelFormat::getRgbAlphaU8(), colorSpace, size);
        REQUIRE_FALSE(comparator.compare(left, right).passed);
    }

    SUBCASE("Images have different color spaces")
    {
        Image right(format, ColorSpace::getSRgb(), size);
        REQUIRE_FALSE(comparator.compare(left, right).passed);
    }

    SUBCASE("Images have different sizes")
    {
        Image right(format, colorSpace, Size{ size.width + 1, size.height });
        REQUIRE_FALSE(comparator.compare(left, right).passed);
    }

    SUBCASE("Images have different row alignment values")
    {
        Image right(format, colorSpace, size, 1);
        REQUIRE(comparator.compare(left, right).passed);
    }
}