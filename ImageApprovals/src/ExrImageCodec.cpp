#ifdef ImageApprovals_CONFIG_WITH_OPENEXR

#include "ExrImageCodec.hpp"
#include <ImageApprovals/Errors.hpp>
#include <cstring>
#include <array>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4996)
#endif

#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfIO.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/half.h>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace ImageApprovals { namespace detail {

namespace {

class InputStramAdapter : public Imf::IStream
{
public:
    InputStramAdapter(const std::string& fileName, std::istream& stream)
        : Imf::IStream(fileName.c_str()), m_stream(stream)
    {}

    bool isMemoryMapped() const override { return false; }

    bool read(char* c, int n) override
    {
        m_stream.read(c, n);

        const auto read_bytes = m_stream.gcount();
        if (read_bytes < n)
        {
            throw ImageApprovalsError("Not enough data");
        }

        return !m_stream.eof();
    }

    char* readMemoryMapped(int) override { throw ImageApprovalsError("Not memory mapped"); }

    Imf::Int64 tellg() override { return m_stream.tellg(); }

    void seekg(Imf::Int64 pos) override { m_stream.seekg(pos); }

    void clear() override { m_stream.clear(); }

private:
    std::istream& m_stream;
};

class OutputStreamAdapter : public Imf::OStream
{
public:
    OutputStreamAdapter(const std::string& fileName, std::ostream& stream)
        : Imf::OStream(fileName.c_str()), m_stream(stream)
    {}

    void write(const char* c, int n) override
    {
        m_stream.write(c, n);
    }

    Imf::Int64 tellp() override { return m_stream.tellp(); }

    void seekp(Imf::Int64 p) override { m_stream.seekp(p); }

private:
    std::ostream& m_stream;
};

}

namespace {

void copyPixels(size_t numChannels, const Size& sz, const Imf::Array2D<Imf::Rgba>& src, uint8_t* dst)
{
    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto srcPixel = src[x][y];

            const float value[]{
                static_cast<float>(srcPixel.r),
                static_cast<float>(srcPixel.g),
                static_cast<float>(srcPixel.b),
                static_cast<float>(srcPixel.a)
            };

            std::memcpy(dst, value, numChannels * 4);
            dst += numChannels * 4;
        }
    }
}

}

std::string ExrImageCodec::getFileExtensionWithDot() const
{
    return ".exr";
}

int ExrImageCodec::getScore(const std::string& extensionWithDot) const
{
    if(extensionWithDot == ".exr")
    {
        return 100;
    }
    
    return -1;
}

int ExrImageCodec::getScore(const PixelFormat& pf, const ColorSpace& cs) const
{
    if(!pf.isF32() || cs != ColorSpace::getLinearSRgb())
    {
        return -1;
    }

    return 100;
}

Image ExrImageCodec::readFromStream(std::istream& stream, const std::string& fileName) const
{
    InputStramAdapter streamAdapter(fileName, stream);

    Imf::RgbaInputFile file(streamAdapter);
    Imath::Box2i dw = file.dataWindow();

    const auto width = dw.max.x - dw.min.x + 1;
    const auto height = dw.max.y - dw.min.y + 1;

    Imf::Array2D<Imf::Rgba> pixels;
    pixels.resizeErase(width, height);

    file.setFrameBuffer(pixels[0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels(dw.min.y, dw.max.y);

    const PixelFormat* fmt = nullptr;

    switch (file.channels())
    {
    case Imf::WRITE_RGB:
        fmt = &PixelFormat::getRgbF32();
        break;
    case Imf::WRITE_RGBA:
        fmt = &PixelFormat::getRgbAlphaF32();
        break;
    default:
        throw ImageApprovalsError("Unsupported pixel format");
    }

    const Size imgSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    Image image(*fmt, ColorSpace::getLinearSRgb(), imgSize, 4);
    copyPixels(fmt->getNumberOfChannels(), imgSize, pixels, image.getPixelData());

    return image;
}

void ExrImageCodec::writeToStream(const ImageView& image, std::ostream& stream, const std::string& fileName) const
{
    const auto& fmt = image.getPixelFormat();

    if (!fmt.isF32())
    {
        throw ImageApprovalsError("EXR codec cannot write non-float pixels");
    }

    if (image.getColorSpace() != ColorSpace::getLinearSRgb())
    {
        throw ImageApprovalsError("EXR codec can write only images with linear color space");
    }

    Imf::RgbaChannels channels{};

    if (fmt == PixelFormat::getRgbF32())
    {
        channels = Imf::WRITE_RGB;
    }
    else if (fmt == PixelFormat::getRgbAlphaF32())
    {
        channels = Imf::WRITE_RGBA;
    }
    else
    {
        throw ImageApprovalsError("Unexpected pixel format");
    }

    OutputStreamAdapter streamAdapter(fileName, stream);

    const auto sz = image.getSize();
    int width = static_cast<int>(sz.width);
    int height = static_cast<int>(sz.height);

    const Imf::Header hdr(
        width, height,
        static_cast<float>(width) / height);

    Imf::RgbaOutputFile file(streamAdapter, hdr, channels);

    Imf::Array2D<Imf::Rgba> pixels;
    pixels.resizeErase(width, height);

    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto srcPixel = image.getPixel(x, y);

            Imf::Rgba dstPixel;
            dstPixel.r = half(srcPixel.r);
            dstPixel.g = half(srcPixel.g);
            dstPixel.b = half(srcPixel.b);
            dstPixel.a = half(srcPixel.a);

            pixels[x][y] = dstPixel;
        }
    }

    file.setFrameBuffer(pixels[0], 1, width);
    file.writePixels(height);
}

} }

#endif // ImageApprovals_CONFIG_WITH_OPENEXR
