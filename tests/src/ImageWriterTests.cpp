#include <doctest/doctest.h>
#include <ImageApprovals.hpp>

using namespace ImageApprovals;

namespace {

struct CustomImageType
{
    const uint32_t width = 16;
    const uint8_t channelValue = 120;
    std::vector<uint8_t> pixels;

    CustomImageType()
    {
        pixels.resize(width * 3, channelValue);
    }

    friend ImageView makeView(const CustomImageType& img)
    {
        return ImageView(PixelFormat::getRgbU8(), ColorSpace::getSRgb(), Size(img.width, 1),
                img.pixels.size(), img.pixels.data());
    }
};

}

TEST_CASE("ImageWriter")
{
    const CustomImageType image;

    ImageWriter writer(image);

    REQUIRE_EQ(writer.getFileExtensionWithDot(), ".png");
}