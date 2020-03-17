#include <doctest/doctest.h>
#include <ImageApprovals/ImageCodec.hpp>
#include <TestsConfig.hpp>
#include <ExrImageCodec.hpp>

using namespace ImageApprovals;

TEST_CASE("ExrImageCodec")
{
    detail::ExrImageCodec codec;

    SUBCASE("Reading")
    {
        const Image image = codec.read(TEST_FILE("exr/v1.7.test.1.exr"));

        REQUIRE_EQ(image.getPixelFormat(), PixelFormat::getRgbF32());
        REQUIRE_EQ(image.getColorSpace(), ColorSpace::getLinearSRgb());
        REQUIRE_EQ(image.getSize(), Size(178, 155));
    }
}