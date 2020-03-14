#include "ColorSpaceUtils.hpp"
#include <ImageApprovals/Errors.hpp>
#include <stdexcept>
#include <cstring>
#include <memory>
#include <array>

namespace ImageApprovals { namespace detail {

bool approxEqual(double a, double b)
{
    return std::abs(a - b) <= 1e-5;
}

bool RgbPrimaries::Primary::approxEqual(const Primary& other) const
{
    return detail::approxEqual(x, other.x)
        && detail::approxEqual(y, other.y);
}

bool RgbPrimaries::approxEqual(const RgbPrimaries& other) const
{
    return r.approxEqual(other.r)
        && g.approxEqual(other.g)
        && b.approxEqual(other.b);
}

RgbPrimaries RgbPrimaries::getSRgbPrimaries()
{
    RgbPrimaries p;
    p.r = Primary(0.64, 0.33);
    p.g = Primary(0.30, 0.60);
    p.b = Primary(0.15, 0.06);
    return p;
}

namespace {

uint32_t fromBigEndian(uint32_t value)
{
    uint32_t result = 0;

    uint8_t beBytes[4];
    std::memcpy(beBytes, &value, 4);
    
    const uint8_t leBytes[4]{ beBytes[3], beBytes[2], beBytes[1], beBytes[0] };
    std::memcpy(&result, leBytes, 4);

    return result;
}

}

bool isSRgbIccProfile(uint32_t profLen, const uint8_t* profData)
{
    if (profLen < 132)
    {
        throw ImageApprovalsError("Incomplete ICC profile data");
    }

    const std::array<char, 4> rgbColorSpace{ 'R', 'G', 'B', ' ' };
    std::array<char, 4> colorSpace;
    std::memcpy(colorSpace.data(), profData + 16, 4);
    if (colorSpace != rgbColorSpace)
    {
        throw ImageApprovalsError("Color space in the ICC profile is not RGB");
    }

    uint32_t numTags = 0;
    std::memcpy(&numTags, profData + 128, 4);
    numTags = fromBigEndian(numTags);
    if ((128 + numTags * 12) > profLen)
    {
        throw ImageApprovalsError("Incomplete ICC profile data");
    }

    struct TagInfo
    {
        std::array<char, 4> signature;
        uint32_t offset;
        uint32_t size;
    };

    for (uint32_t tagIndex = 0; tagIndex < numTags; ++tagIndex)
    {
        TagInfo info{};
        std::memcpy(&info, profData + 132 + tagIndex * sizeof(TagInfo), sizeof(TagInfo));
        info.offset = fromBigEndian(info.offset);
        info.size = fromBigEndian(info.size);

        if ((info.offset + info.size) > profLen)
        {
            throw ImageApprovalsError("Incomplete ICC profile data");
        }

        // profileDescriptionTag 
        const std::array<char, 4> descSig{ 'd', 'e', 's', 'c' };
        if (info.signature == descSig)
        {
            const uint8_t* descPtr = profData + info.offset;

            uint32_t strLen = 0, strOffset = 0;

            std::memcpy(&strLen, descPtr + 20, 4);
            strLen = fromBigEndian(strLen);

            std::memcpy(&strOffset, descPtr + 24, 4);
            strOffset = fromBigEndian(strOffset);

            std::unique_ptr<char[]> str(new char[(strLen / 2) + 1]);
            str[strLen / 2] = '\0';

            for (uint32_t i = 0; i < strLen / 2; ++i)
            {
                str[i] = static_cast<char>((descPtr + strOffset)[1 + i * 2]);
            }

            if (std::strstr(str.get(), "sRGB") != nullptr)
            {
                return true;
            }
        }
    }

    return false;
}

const ColorSpace* detectColorSpace(const RgbPrimaries& primaries, double gamma)
{
    const auto sRgbPrimaries = RgbPrimaries::getSRgbPrimaries();

    if (!primaries.approxEqual(sRgbPrimaries))
    {
        return nullptr;
    }

    if (approxEqual(gamma, 1.0))
    {
        return &ColorSpace::getLinearSRgb();
    }
    else if (approxEqual(gamma, 1.0 / 2.2))
    {
        return &ColorSpace::getSRgb();
    }

    return nullptr;
}

} }