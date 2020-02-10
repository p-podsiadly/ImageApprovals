#ifndef IMAGEAPPROVALS_COLORSPACEUTILS_HPP_INCLUDED
#define IMAGEAPPROVALS_COLORSPACEUTILS_HPP_INCLUDED

#include <ImageApprovals/ColorSpace.hpp>
#include <cstdint>
#include <cmath>

namespace ImageApprovals {

struct RgbPrimaries
{
    struct Primary
    {
        double x = 0.0, y = 0.0;

        bool approxEqual(const Primary& other) const;
    };

    Primary r, g, b;

    bool approxEqual(const RgbPrimaries& other) const;

    static RgbPrimaries getSRgbPrimaries();
};

bool isSRgbIccProfile(uint32_t profLen, const uint8_t* profData);

const ColorSpace* detectColorSpace(const RgbPrimaries& primaries, double gamma);

}

#endif // IMAGEAPPROVALS_COLORSPACEUTILS_HPP_INCLUDED