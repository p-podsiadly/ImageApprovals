#ifndef IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED
#define IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED

#include <cstdint>
#include <iosfwd>

namespace ImageApprovals {

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
    {
        return(r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
    }

    bool operator !=(const RGBA& rhs) const
    {
        return !(*this == rhs);
    }
};

class PixelFormat
{
public:
    virtual ~PixelFormat() = default;

    bool operator ==(const PixelFormat& rhs) const
    { return this == &rhs; }

    bool operator !=(const PixelFormat& rhs) const
    { return !(*this == rhs); }

    virtual const char* getName() const = 0;

    virtual size_t getNumberOfChannels() const = 0;
    virtual size_t getPixelStride() const = 0;
    virtual bool isU8() const = 0;
    virtual bool isF32() const = 0;

    const uint8_t* decode(const uint8_t* begin, const uint8_t* end, RGBA& outRgba) const;

    static const PixelFormat& getGrayU8();
    static const PixelFormat& getGrayAlphaU8();

    static const PixelFormat& getRgbU8();
    static const PixelFormat& getRgbAlphaU8();
    
    static const PixelFormat& getRgbF32();
    static const PixelFormat& getRgbAlphaF32();

protected:
    virtual void decode(const uint8_t* begin, RGBA& outRgba) const = 0;
};

std::ostream& operator <<(std::ostream& stream, const PixelFormat& format);
std::ostream& operator <<(std::ostream& stream, const RGBA& rgba);

}

#endif // IMAGEAPPROVALS_PIXELFORMAT_HPP_INCLUDED