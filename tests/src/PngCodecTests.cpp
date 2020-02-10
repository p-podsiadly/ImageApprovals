#include <doctest/doctest.h>
#include <ImageApprovals/ImageCodec.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;

TEST_CASE("PngCodec")
{
    SUBCASE("Reading 8 bit GA")
    {
        const Image img = ImageCodec::read(TEST_FILE("png/basi4a08.png"));

        REQUIRE_EQ(img.getPixelFormat(), PixelFormat::getGrayAlphaU8());
        REQUIRE_EQ(img.getColorSpace(), ColorSpace::getLinearSRgb());
        REQUIRE_EQ(img.getSize(), Size{ 32, 32 });
    }

    SUBCASE("gimp_sRGB.png")
    {
        const Image img = ImageCodec::read(TEST_FILE("png/gimp_sRGB.png"));

        REQUIRE_EQ(img.getColorSpace(), ColorSpace::getSRgb());
    }

    SUBCASE("gimp_sRGB_with_gamma.png")
    {
        const Image img = ImageCodec::read(TEST_FILE("png/gimp_sRGB_with_gamma.png"));

        REQUIRE_EQ(img.getColorSpace(), ColorSpace::getSRgb());
    }

    SUBCASE("paint.png")
    {
        const Image img = ImageCodec::read(TEST_FILE("png/paint.png"));

        REQUIRE_EQ(img.getColorSpace(), ColorSpace::getSRgb());
    }
}