#include <doctest/doctest.h>
#include <ImageApprovals/Image.hpp>
#include <stdexcept>

using namespace ImageApprovals;

TEST_CASE("Image")
{
    SUBCASE("Image constructor throws when called with incorrect arguments")
    {
        auto makeImage = [](const Size& sz, uint32_t rowAlignment) {
            return Image(PixelFormat::getRgbU8(), ColorSpace::getLinearSRgb(), sz, rowAlignment);
        };

        REQUIRE_THROWS_AS(makeImage(Size(0, 1), 1), std::logic_error);
        REQUIRE_THROWS_AS(makeImage(Size(1, 0), 1), std::logic_error);
        REQUIRE_THROWS_AS(makeImage(Size(1, 1), 0), std::logic_error);
    }

    SUBCASE("getPixelFormat/getColorSpace throws when called on an empty Image")
    {
        Image emptyImage;

        REQUIRE_THROWS_AS(emptyImage.getPixelFormat(), std::logic_error);
        REQUIRE_THROWS_AS(emptyImage.getColorSpace(), std::logic_error);
    }
}