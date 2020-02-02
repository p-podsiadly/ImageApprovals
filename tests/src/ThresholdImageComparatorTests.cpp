#include <doctest/doctest.h>
#include <ImageApprovals.hpp>

using namespace ImageApprovals;

TEST_CASE("ThresholdImageComparator")
{
    ThresholdImageComparator comparator(AbsThreshold(0.004), Percent(5.0));

    const PixelFormat format{ PixelLayout::RGB, PixelDataType::UInt8 };
    const auto& colorSpace = ColorSpace::getLinear();
    const Size size{ 20, 1 };

    Image left(format, colorSpace, size);
    
    SUBCASE("Differences are within thresholds")
    {
        Image right(format, colorSpace, size);
        auto pixels = right.getPixelData();
        pixels[6] = 2;

        REQUIRE(comparator.compare(left.getView(), right.getView()).passed);
    }

    SUBCASE("Differences are not within thresholds")
    {
        Image right(format, colorSpace, size);
        auto pixels = right.getPixelData();
        pixels[6] = 2;
        pixels[10] = 2;
        pixels[14] = 2;
        
        REQUIRE_FALSE(comparator.compare(left.getView(), right.getView()).passed);
    }

    SUBCASE("Images have different pixel formats")
    {
        Image right({ PixelLayout::RGBA, format.dataType }, colorSpace, size);
        REQUIRE_FALSE(comparator.compare(left.getView(), right.getView()).passed);

        right = Image({ format.layout, PixelDataType::Float }, colorSpace, size);
        REQUIRE_FALSE(comparator.compare(left.getView(), right.getView()).passed);
    }

    SUBCASE("Images have different color spaces")
    {
        Image right(format, ColorSpace::getSRGB(), size);
        REQUIRE_FALSE(comparator.compare(left.getView(), right.getView()).passed);
    }

    SUBCASE("Images have different sizes")
    {
        Image right(format, colorSpace, Size{ size.width + 1, size.height });
        REQUIRE_FALSE(comparator.compare(left.getView(), right.getView()).passed);
    }

    SUBCASE("Images have different row alignment values")
    {
        Image right(format, colorSpace, size, 1);
        REQUIRE(comparator.compare(left.getView(), right.getView()).passed);
    }
}