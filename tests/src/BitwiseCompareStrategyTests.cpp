#include <doctest/doctest.h>
#include <ImageApprovals.hpp>

using namespace ImageApprovals;

TEST_CASE("BitwiseCompareStrategy")
{
    BitwiseCompareStrategy strategy;

    const PixelFormat& format = PixelFormat::getRgbU8();
    const auto& colorSpace = ColorSpace::getLinearSRgb();
    const Size size{ 20, 1 };

    Image left(format, colorSpace, size);

    SUBCASE("Pixels are equal")
    {
        Image right(format, colorSpace, size);

        REQUIRE(strategy.compare(left, right).passed);
    }

    SUBCASE("Pixels are different")
    {
        Image right(format, colorSpace, size);
        right.getPixelData()[8] = 1;

        REQUIRE_FALSE(strategy.compare(left, right).passed);
    }
}