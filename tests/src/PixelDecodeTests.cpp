#include <doctest/doctest.h>
#include <ImageApprovals/PixelFormat.hpp>
#include <stdexcept>

using namespace doctest;
using namespace ImageApprovals;

TEST_CASE("decode")
{
    SUBCASE("LuminanceAlpha, UInt8")
    {
        const PixelFormat fmt{ PixelLayout::LuminanceAlpha, PixelDataType::UInt8 };

        const uint8_t pixel[]{ 127, 200 };

        const RGBA value = decode(fmt, pixel, pixel + 2);

        REQUIRE_EQ(value.r, Approx(0.4980392));
        REQUIRE_EQ(value.g, Approx(0.4980392));
        REQUIRE_EQ(value.b, Approx(0.4980392));
        REQUIRE_EQ(value.a, Approx(0.7843137));
    }

    SUBCASE("LuminanceAlpha, Float")
    {
        const PixelFormat fmt{ PixelLayout::LuminanceAlpha, PixelDataType::Float };

        const float pixel[]{ 1.25f, 0.5f };
        uint8_t pixelBytes[8];
        std::memcpy(pixelBytes, pixel, 8);

        const RGBA value = decode(fmt, pixelBytes, pixelBytes + 8);

        REQUIRE_EQ(value, RGBA{ 1.25f, 1.25f, 1.25f, 0.5f });
    }

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

    SUBCASE("RGB, Float")
    {
        const PixelFormat fmt{ PixelLayout::RGB, PixelDataType::Float };

        const float pixel[]{ 0.0f, 1.0f, 0.375f };
        uint8_t pixelBytes[12];
        std::memcpy(pixelBytes, pixel, 12);

        const RGBA value = decode(fmt, pixelBytes, pixelBytes + 12);

        REQUIRE_EQ(value, RGBA{ pixel[0], pixel[1], pixel[2], 1.0f });
    }

    SUBCASE("Bounds checking")
    {
        const PixelFormat fmt{ PixelLayout::RGB, PixelDataType::UInt8 };

        const uint8_t pixel[]{
            0,
            255,
            127,
            0, 0, 0
        };

        REQUIRE_THROWS_AS(decode(fmt, pixel, pixel + 2), std::out_of_range);
        REQUIRE_THROWS_AS(decode(fmt, pixel + 2, pixel), std::out_of_range);

        REQUIRE_NOTHROW(decode(fmt, pixel, pixel + 6));
    }
}