#include <doctest/doctest.h>
#include <ImageApprovals/ImageCodec.hpp>
#include <TestsConfig.hpp>

using namespace ImageApprovals;

TEST_CASE("PngCodec")
{
    SUBCASE("Reading 8 bit GA")
    {
        const Image img = ImageCodec::read(TEST_FILE("png/basi4a08.png"));

        REQUIRE_EQ(img.getPixelFormat(), PixelFormat{ PixelLayout::LuminanceAlpha, PixelDataType::UInt8 });
        REQUIRE_EQ(img.getColorSpace(), ColorSpace::getLinear());
        REQUIRE_EQ(img.getSize(), Size{ 32, 32 });
    }
}