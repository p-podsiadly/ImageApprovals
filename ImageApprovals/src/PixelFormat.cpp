#include <ImageApprovals/PixelFormat.hpp>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <ostream>
#include <array>

namespace ImageApprovals {

namespace {

float clamp(float x, float minX, float maxX)
{
    return std::min(maxX, std::max(minX, x));
}

template<typename T, size_t N>
auto normalize(const std::array<T, N>& src, float maxValue)
{
    std::array<float, N> dst;

    for (size_t i = 0; i < N; ++i)
    {
        dst[i] = clamp(src[i] / maxValue, 0.0f, 1.0f);
    }

    return dst;
}

template<size_t NumChannels, typename ChannelType>
struct GenericPixelFormat : PixelFormat
{
    size_t getNumberOfChannels() const override { return NumChannels; }
    size_t getPixelStride() const override { return NumChannels * sizeof(ChannelType); }

    bool isU8() const override { return std::is_same<ChannelType, uint8_t>::value; }
    bool isF32() const override { return std::is_same<ChannelType, float>::value; }

    std::array<ChannelType, NumChannels> decodeBytes(const uint8_t* src) const
    {
        std::array<ChannelType, NumChannels> dst;
        std::memcpy(dst.data(), src, NumChannels * sizeof(ChannelType));
        return dst;
    }
};

struct GrayU8PixelFormat : GenericPixelFormat<1, uint8_t>
{
    const char* getName() const override { return "GrayU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f)[0];
        outRgba = RGBA(v, v, v, 1);
    }
};

struct GrayAlphaU8PixelFormat : GenericPixelFormat<2, uint8_t>
{
    const char* getName() const override { return "GrayAlphaU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f);
        outRgba = RGBA(v[0], v[0], v[0], v[1]);
    }
};

struct RgbU8PixelFormat : GenericPixelFormat<3, uint8_t>
{
    const char* getName() const override { return "RgbU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f);
        outRgba = RGBA(v[0], v[1], v[2], 1);
    }
};

struct RgbAlphaU8PixelFormat : GenericPixelFormat<4, uint8_t>
{
    const char* getName() const override { return "RgbAlphaU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f);
        outRgba = RGBA(v[0], v[1], v[2], v[3]);
    }
};

struct RgbF32PixelFormat : GenericPixelFormat<3, float>
{
    const char* getName() const override { return "RgbF32"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = decodeBytes(begin);
        outRgba = RGBA(v[0], v[1], v[2], 1);
    }
};

struct RgbAlphaF32PixelFormat : GenericPixelFormat<4, float>
{
    const char* getName() const override { return "RgbAlphaF32"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = decodeBytes(begin);
        outRgba = RGBA(v[0], v[1], v[2], v[3]);
    }
};

}

const uint8_t* PixelFormat::decode(const uint8_t* begin, const uint8_t* end, RGBA& outRgba) const
{
    const auto stride = getPixelStride();
    if (begin + stride > end)
    {
        throw std::out_of_range("begin + stride > end");
    }

    decode(begin, outRgba);
    return begin + stride;
}

const PixelFormat& PixelFormat::getGrayU8()
{
    static const GrayU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getGrayAlphaU8()
{
    static const GrayAlphaU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbU8()
{
    static const RgbU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbAlphaU8()
{
    static const RgbAlphaU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbF32()
{
    static const RgbF32PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbAlphaF32()
{
    static const RgbAlphaF32PixelFormat instance;
    return instance;
}

std::ostream& operator <<(std::ostream& stream, const PixelFormat& format)
{
    stream << format.getName();
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const RGBA& rgba)
{
    stream << "(" << rgba.r << ", " << rgba.g << ", " << rgba.b << ", " << rgba.a << ")";
    return stream;
}

}