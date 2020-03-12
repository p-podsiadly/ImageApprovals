#include <doctest/doctest.h>
#include <ImageApprovals.hpp>
#include <stdexcept>
#include <cstring>

using namespace doctest;
using namespace ImageApprovals;

namespace {

struct ApproxRGBA
{
    const RGBA rgba;

    ApproxRGBA(float r, float g, float b, float a)
        : rgba(r, g, b, a)
    {}

    friend bool operator ==(const RGBA& lhs, const ApproxRGBA& rhs)
    {
        return lhs.r == Approx(rhs.rgba.r)
            && lhs.g == Approx(rhs.rgba.g)
            && lhs.b == Approx(rhs.rgba.b)
            && lhs.a == Approx(rhs.rgba.a);
    }

    friend std::ostream& operator <<(std::ostream& stream, const ApproxRGBA& rhs)
    {
        stream << "Approx" << rhs.rgba;
        return stream;
    }
};

}

TEST_CASE("PixelFormat::decode")
{
    SUBCASE("GrayU8")
    {
        const PixelFormat& fmt = PixelFormat::getGrayU8();

        const uint8_t pixel[]{
            0,
            127,
            255
        };

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixel, pixel + 3, value), pixel + 1);
        REQUIRE_EQ(value, RGBA(0.0f, 0.0f, 0.0f, 1.0f));

        REQUIRE_EQ(fmt.decode(pixel + 1, pixel + 3, value), pixel + 2);
        REQUIRE_EQ(value, ApproxRGBA(0.4980392f, 0.4980392f, 0.4980392f, 1.0f));

        REQUIRE_EQ(fmt.decode(pixel + 2, pixel + 3, value), pixel + 3);
        REQUIRE_EQ(value, RGBA(1.0f, 1.0f, 1.0f, 1.0f));
    }

    SUBCASE("GrayAlphaU8")
    {
        const PixelFormat& fmt = PixelFormat::getGrayAlphaU8();

        const uint8_t pixel[]{
            127, 200
        };

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixel, pixel + 2, value), pixel + 2);
        REQUIRE_EQ(value, ApproxRGBA(0.4980392f, 0.4980392f, 0.4980392f, 0.78431372f));
    }

    SUBCASE("RgbU8")
    {
        const PixelFormat& fmt = PixelFormat::getRgbU8();

        const uint8_t pixel[]{
            0,
            255,
            127
        };

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixel, pixel + 3, value), pixel + 3);
        REQUIRE_EQ(value, ApproxRGBA(0.0f, 1.0f, 0.4980392f, 1.0f));
    }

    SUBCASE("RgbF32")
    {
        const PixelFormat& fmt = PixelFormat::getRgbF32();

        const float pixel[]{ 0.0f, 1.0f, 0.375f };
        uint8_t pixelBytes[12];
        std::memcpy(pixelBytes, pixel, 12);

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixelBytes, pixelBytes + 12, value), pixelBytes + 12);
        REQUIRE_EQ(value, RGBA(pixel[0], pixel[1], pixel[2], 1.0f));
    }

    SUBCASE("RgbaU8")
    {
        const PixelFormat& fmt = PixelFormat::getRgbAlphaU8();

        const uint8_t pixel[]{
            0,
            127,
            255,
            200
        };

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixel, pixel + 4, value), pixel + 4);
        REQUIRE_EQ(value, ApproxRGBA(0.0f, 0.4980392f, 1.0f, 0.78431372f));
    }

    SUBCASE("RgbaF32")
    {
        const PixelFormat& fmt = PixelFormat::getRgbAlphaF32();

        const float pixel[]{
            0.125f,
            0.5f,
            1.25f,
            0.625f,
        };

        uint8_t pixelBytes[16];
        std::memcpy(pixelBytes, pixel, 16);

        RGBA value;
        REQUIRE_EQ(fmt.decode(pixelBytes, pixelBytes + 16, value), pixelBytes + 16);
        REQUIRE_EQ(value, RGBA(0.125f, 0.5f, 1.25f, 0.625f));
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

        REQUIRE_THROWS_AS(fmt.decode(pixel, pixel + 2, value), ImageApprovalsError);
        REQUIRE_THROWS_AS(fmt.decode(pixel + 2, pixel, value), ImageApprovalsError);
        REQUIRE_NOTHROW(fmt.decode(pixel, pixel + 6, value));
    }
}