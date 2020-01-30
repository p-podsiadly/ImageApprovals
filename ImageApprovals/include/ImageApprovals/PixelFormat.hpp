#ifndef IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED
#define IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED

#include <cstdint>
#include <iosfwd>

namespace ImageApprovals {

enum class PixelLayout
{
    Luminance,
    LuminanceAlpha,
    RGB,
    RGBA
};

enum class PixelDataType
{
    UInt8,
    Float
};

struct PixelFormat
{
    PixelLayout layout = PixelLayout::Luminance;
    PixelDataType dataType = PixelDataType::UInt8;
    
    constexpr PixelFormat() = default;
    constexpr PixelFormat(const PixelFormat&) = default;
    
    constexpr PixelFormat(PixelLayout layout, PixelDataType dataType)
        : layout(layout), dataType(dataType)
    {}

    PixelFormat& operator =(const PixelFormat&) = default;

    bool operator ==(const PixelFormat& rhs) const
    { return (layout == rhs.layout) && (dataType == rhs.dataType); }

    bool operator !=(const PixelFormat& rhs) const
    { return !(*this == rhs); }
};

std::size_t getPixelStride(const PixelFormat& format);

struct RGBA
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    constexpr RGBA() = default;
    constexpr RGBA(const RGBA&) = default;

    constexpr RGBA(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a)
    {}

    RGBA& operator =(const RGBA&) = default;

    bool operator ==(const RGBA& rhs) const
    { return(r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a); }

    bool operator !=(const RGBA& rhs) const
    { return !(*this == rhs); }
};

RGBA decode(const PixelFormat& format, const uint8_t* begin, const uint8_t* end);

std::ostream& operator <<(std::ostream& stream, PixelLayout pixelLayout);
std::ostream& operator <<(std::ostream& stream, PixelDataType pixelDataType);
std::ostream& operator <<(std::ostream& stream, const PixelFormat& format);
std::ostream& operator <<(std::ostream& stream, const RGBA& rgba);

}

#endif // IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED