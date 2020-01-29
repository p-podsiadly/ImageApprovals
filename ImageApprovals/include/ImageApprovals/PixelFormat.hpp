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
};

RGBA decode(const PixelFormat& format, const uint8_t* begin, const uint8_t* end);

std::ostream& operator <<(std::ostream& stream, PixelLayout pixelLayout);
std::ostream& operator <<(std::ostream& stream, PixelDataType pixelDataType);
std::ostream& operator <<(std::ostream& stream, const PixelFormat& format);

}

#endif // IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED