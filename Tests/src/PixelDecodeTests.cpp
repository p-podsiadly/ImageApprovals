#include <doctest/doctest.h>
#include <ImageApprovals/PixelFormat.hpp>

using namespace doctest;
using namespace ImageApprovals;

TEST_CASE("decode")
{
    SUBCASE("RGB, UInt8")
    {
        const PixelFormat fmt{ PixelLayout::RGB, PixelDataType::UInt8 };

        const uint8_t pixel[]{
            0,
            255,
            127
        };

        const RGBA value = decode(fmt, pixel, pixel + 3);

        REQUIRE_EQ(value.r, 0.0f);
        REQUIRE_EQ(value.g, 1.0f);
        REQUIRE_EQ(value.b, Approx(0.4980392));
        REQUIRE_EQ(value.a, 1.0f);
    }
}