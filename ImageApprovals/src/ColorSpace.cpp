#include <ImageApprovals/ColorSpace.hpp>
#include <ostream>

namespace ImageApprovals {

namespace detail {

struct LinearColorSpace : ColorSpace
{
    const char* getName() const override
    { return "Linear sRGB"; }
};

struct SRGBColorSpace : ColorSpace
{
    const char* getName() const override
    { return "sRGB"; }
};

}

const ColorSpace& ColorSpace::getLinearSRgb()
{
    static const detail::LinearColorSpace instance;
    return instance;
}

const ColorSpace& ColorSpace::getSRgb()
{
    static const detail::SRGBColorSpace instance;
    return instance;
}

std::ostream& operator <<(std::ostream& stream, const ColorSpace& colorSpace)
{
    return stream << colorSpace.getName();
}

}