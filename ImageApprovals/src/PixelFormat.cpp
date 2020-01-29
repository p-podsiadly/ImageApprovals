#include <ImageApprovals/PixelFormat.hpp>
#include <stdexcept>
#include <ostream>

namespace ImageApprovals {

namespace {

struct LayoutProps
{
    PixelLayout layout;
    std::size_t numComponents;
    const char* str;
};

struct DataTypeProps
{
    PixelDataType dataType;
    std::size_t stride;
    const char* str;
};

const LayoutProps& getProps(PixelLayout layout)
{
    static const LayoutProps layoutProps[]{
        { PixelLayout::Luminance,      1, "Luminance"      },
        { PixelLayout::LuminanceAlpha, 2, "LuminanceAlpha" },
        { PixelLayout::RGB,            3, "RGB"            },
        { PixelLayout::RGBA,           4, "RGBA"           }
    };

    for (const auto& props : layoutProps)
    {
        if (props.layout == layout)
        {
            return props;
        }
    }

    throw std::runtime_error("Invalid PixelLayout value");
}

const DataTypeProps& getProps(PixelDataType dataType)
{
    static const DataTypeProps dataTypeProps[]{
        { PixelDataType::UInt8, 1, "UInt8" },
        { PixelDataType::Float, 4, "Float" }
    };

    for (const auto& props : dataTypeProps)
    {
        if (props.dataType == dataType)
        {
            return props;
        }
    }

    throw std::runtime_error("Invalid PixelDataType value");
}

}

std::size_t getPixelStride(const PixelFormat& format)
{
    const auto& layoutProps = getProps(format.layout);
    const auto& dataTypeProps = getProps(format.dataType);

    return layoutProps.numComponents * dataTypeProps.stride;
}

namespace {

template<typename DecodeValue>
RGBA decode(PixelLayout layout, const uint8_t* data, DecodeValue&& decodeValue)
{
    RGBA result{ 0.0f, 0.0f, 0.0f, 1.0f };
    result.r = decodeValue(&data);

    switch (layout)
    {
    case PixelLayout::Luminance:
    case PixelLayout::LuminanceAlpha:
        result.g = result.b = result.r;
        break;
    case PixelLayout::RGB:
    case PixelLayout::RGBA:
        result.g = decodeValue(&data);
        result.b = decodeValue(&data);
        break;
    }

    switch (layout)
    {
    case PixelLayout::LuminanceAlpha:
    case PixelLayout::RGBA:
        result.a = decodeValue(&data);
        break;
    default:
        break;
    }

    return result;
}

}

RGBA decode(const PixelFormat& format, const uint8_t* begin, const uint8_t* end)
{
    const auto stride = getPixelStride(format);

    if (begin + stride > end)
    {
        throw std::out_of_range("not enough data to decode a pixel");
    }

    switch (format.dataType)
    {
    case PixelDataType::UInt8:
        return decode(format.layout, begin,
            [](const uint8_t** ptr) { return *((*ptr)++) / 255.0f; });
    case PixelDataType::Float:
        return decode(format.layout, begin,
            [](const uint8_t** ptr) {
                float v;
                std::memcpy(&v, *ptr, 4);
                (*ptr) += 4;
                return v;
            });
    }

    throw std::runtime_error("Invalid value of PixelFormat::dataType");
}

std::ostream& operator <<(std::ostream& stream, PixelLayout pixelLayout)
{
    return stream << getProps(pixelLayout).str;
}

std::ostream& operator <<(std::ostream& stream, PixelDataType pixelDataType)
{
    return stream << getProps(pixelDataType).str;
}

std::ostream& operator <<(std::ostream& stream, const PixelFormat& format)
{
    const auto& layoutProps = getProps(format.layout);
    const auto& dataTypeProps = getProps(format.dataType);

    stream << "{" << layoutProps.str << ", " << dataTypeProps.str << "}";

    return stream;
}

}