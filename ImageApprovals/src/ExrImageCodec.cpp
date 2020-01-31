#include <ImageApprovals/ImageCodec.hpp>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfIO.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/half.h>

namespace ImageApprovals {

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
            throw std::runtime_error("not eough data");
        }

        return !m_stream.eof();
    }

    char* readMemoryMapped(int) override { throw std::runtime_error("not memory mapped"); }

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

class ExrImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override
    {
        return ".exr";
    }

    bool canRead(std::istream& stream, const std::string& fileName) const override
    {
        InputStramAdapter streamAdapter(fileName, stream);

        try
        {
            Imf::RgbaInputFile file(streamAdapter);
        }
        catch (...)
        {
            stream.seekg(0);
            return false;
        }

        stream.seekg(0);
        return true;
    }

    Image read(std::istream& stream, const std::string& fileName) const override
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

        const PixelFormat fmt(PixelLayout::RGBA, PixelDataType::Float);
        const Size imgSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

        Image image(fmt, ColorSpace::getLinear(), imgSize);
        for (uint32_t y = 0; y < imgSize.height; ++y)
        {
            float* dst = reinterpret_cast<float*>(image.getRowPointer(y));

            for (uint32_t x = 0; x < imgSize.width; ++x)
            {
                const auto pixel = pixels[x][y];

                dst[0] = static_cast<float>(pixel.r);
                dst[1] = static_cast<float>(pixel.g);
                dst[2] = static_cast<float>(pixel.b);
                dst[3] = static_cast<float>(pixel.a);

                dst += 4;
            }
        }

        return image;
    }

    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override
    {
        const auto fmt = image.getPixelFormat();
        if (fmt.dataType != PixelDataType::Float)
        {
            throw std::runtime_error("EXR codec cannot write non-float pixels");
        }

        if (image.getColorSpace() != ColorSpace::getLinear())
        {
            throw std::runtime_error("EXR codec can write only images with linear color space");
        }

        OutputStreamAdapter streamAdapter(fileName, stream);

        const auto sz = image.getSize();
        int width = static_cast<int>(sz.width);
        int height = static_cast<int>(sz.height);

        const Imf::Header hdr(
            width, height,
            static_cast<float>(width) / height);

        Imf::RgbaOutputFile file(streamAdapter, hdr);

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
};

const ImageCodec& ImageCodec::getExrCodec()
{
    static const ExrImageCodec instance;
    return instance;
}

}