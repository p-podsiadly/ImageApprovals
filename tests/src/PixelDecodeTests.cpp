#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <stdexcept>
#include <cstring>

using namespace doctest;
using namespace ImageApprovals;

TEST_CASE("decode")
{
    SUBCASE("RGB, UInt8")
    {
        const PixelFormat& fmt = PixelFormat::getRgbU8();

        const uint8_t pixel[]{
            0,
            255,
            127
        };

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixel, pixel + 3, value), pixel + 3);
        REQUIRE_EQ(value.r, 0.0f);
        REQUIRE_EQ(value.g, 1.0f);
        REQUIRE_EQ(value.b, Approx(0.4980392));
        REQUIRE_EQ(value.a, 1.0f);
    }

    SUBCASE("RGB, Float")
    {
        const PixelFormat& fmt = PixelFormat::getRgbF32();

        const float pixel[]{ 0.0f, 1.0f, 0.375f };
        uint8_t pixelBytes[12];
        std::memcpy(pixelBytes, pixel, 12);

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixelBytes, pixelBytes + 12, value), pixelBytes + 12);
        REQUIRE_EQ(value, RGBA{ pixel[0], pixel[1], pixel[2], 1.0f });
    }

    SUBCASE("Bounds checking")
    {
        const PixelFormat& fmt = PixelFormat::getRgbU8();

        const uint8_t pixel[]{
            0,
            255,
            127,
            0, 0, 0
        };

        RGBA value;

        REQUIRE_THROWS_AS(fmt.decode(pixel, pixel + 2, value), std::out_of_range);
        REQUIRE_THROWS_AS(fmt.decode(pixel + 2, pixel, value), std::out_of_range);
        REQUIRE_NOTHROW(fmt.decode(pixel, pixel + 6, value));
    }
}