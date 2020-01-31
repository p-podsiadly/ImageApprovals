#include <ImageApprovals/ImageCodec.hpp>
#include <png.h>
#include <functional>
#include <cstring>
#include <istream>
#include <ostream>

namespace ImageApprovals {

namespace {

class OnExit
{
public:
    OnExit(std::function<void()> fn)
        : m_fn(std::move(fn))
    {}

    OnExit(const OnExit&) = delete;
    OnExit(OnExit&&) = delete;

    ~OnExit() noexcept
    {
        m_fn();
    }

private:
    std::function<void()> m_fn;
};

}

struct PngImageCodec : ImageCodec
{
    std::string getFileExtensionWithDot() const override
    {
        return ".png";
    }

    bool canRead(std::istream& stream, const std::string&) const override
    {
        png_byte signature[8];
        std::fill_n(signature, 8, 0);

        stream.read(reinterpret_cast<char*>(signature), 8);
        stream.seekg(0);

        return (png_sig_cmp(signature, 0, 8) == 0);
    }

    Image read(std::istream& stream, const std::string&) const override
    {
        Image image;
        png_struct* png = nullptr;
        png_info* info = nullptr;

        OnExit onExit([&]() {
            if (png)
            {
                png_destroy_read_struct(&png, info ? &info : nullptr, nullptr);
            }
        });

        if (!(png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)))
        {
            throw std::runtime_error("failed to allocate PNG read struct");
        }
        
        if (!(info = png_create_info_struct(png)))
        {
            throw std::runtime_error("failed to allocate PNG info struct");
        }

        if (setjmp(png_jmpbuf(png)))
        {
            throw std::runtime_error("failed to read PNG image");
        }

        png_set_read_fn(png, &stream, &PngImageCodec::readBytes);

        png_read_png(png, info, 0, nullptr);

        PixelFormat format;
        const ColorSpace* colorSpace = &ColorSpace::getLinear();
        const Size imgSize{ png_get_image_width(png, info), png_get_image_height(png, info) };

        const int pngBitDepth = png_get_bit_depth(png, info);
        const int pngColorType = png_get_color_type(png, info);

        if (pngBitDepth != 8)
        {
            throw std::runtime_error("unsupported PNG bit depth");
        }

        switch (pngColorType)
        {
        case PNG_COLOR_TYPE_GRAY:
            format.layout = PixelLayout::Luminance;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            format.layout = PixelLayout::LuminanceAlpha;
            break;
        case PNG_COLOR_TYPE_RGB:
            format.layout = PixelLayout::RGB;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            format.layout = PixelLayout::RGBA;
            break;
        default:
            throw std::runtime_error("unsupported PNG color type");
        }

        int pngSRGBIntent = 0;
        if (png_get_sRGB(png, info, &pngSRGBIntent) == PNG_INFO_sRGB)
        {
            colorSpace = &ColorSpace::getSRGB();
        }

        image = Image(format, *colorSpace, imgSize);

        png_byte** rowPointers = png_get_rows(png, info);
        const size_t rowSize = getPixelStride(format) * imgSize.width;
        for (uint32_t y = 0; y < imgSize.height; ++y)
        {
            const png_byte* srcRow = rowPointers[y];
            uint8_t* dstRow = image.getRowPointer(y);
            std::memcpy(dstRow, srcRow, rowSize);
        }

        return image;
    }

    void write(const ImageView& image, std::ostream& stream, const std::string&) const override
    {
        if (image.getPixelFormat().dataType != PixelDataType::UInt8)
        {
            throw std::runtime_error("unable to write the image to PNG file");
        }

        png_struct* png = nullptr;
        png_info* info = nullptr;
        
        std::unique_ptr<png_const_bytep[]> rowPointers;

        OnExit onExit([&]() {
            if (png)
            {
                png_destroy_write_struct(&png, info ? &info : nullptr);
            }
        });

        if (!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)))
        {
            throw std::runtime_error("failed to allocate PNG write struct");
        }

        if (!(info = png_create_info_struct(png)))
        {
            throw std::runtime_error("failed to allocate PNG info struct");
        }

        if (setjmp(png_jmpbuf(png)))
        {
            throw std::runtime_error("failed to write PNG image");
        }

        png_set_write_fn(png, &stream, &PngImageCodec::writeBytes, &PngImageCodec::flush);

        int pngColorType = 0;
        switch (image.getPixelFormat().layout)
        {
        case PixelLayout::Luminance:
            pngColorType = PNG_COLOR_TYPE_GRAY;
            break;
        case PixelLayout::LuminanceAlpha:
            pngColorType = PNG_COLOR_TYPE_GRAY_ALPHA;
            break;
        case PixelLayout::RGB:
            pngColorType = PNG_COLOR_TYPE_RGB;
            break;
        case PixelLayout::RGBA:
            pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
        default:
            throw std::runtime_error("unsupported PixelLayout value");
        }

        const auto sz = image.getSize();
        png_set_IHDR(
            png, info, sz.width, sz.height,
            8, pngColorType,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT);

        if (image.getColorSpace() == ColorSpace::getSRGB())
        {
            png_set_sRGB(png, info, PNG_sRGB_INTENT_RELATIVE);
        }
        else if (image.getColorSpace() != ColorSpace::getLinear())
        {
            throw std::runtime_error("unsupported ColorSpace");
        }

        rowPointers.reset(new png_const_bytep[sz.height]);
        for (uint32_t y = 0; y < sz.height; ++y)
        {
            rowPointers[y] = reinterpret_cast<const png_byte*>(image.getRowPointer(y));
        }

        png_set_rows(png, info, const_cast<png_bytepp>(rowPointers.get()));

        png_write_png(png, info, 0, nullptr);
    }

    static void PNGCBAPI readBytes(png_struct* png, png_byte* data, size_t len)
    {
        auto& stream = *reinterpret_cast<std::istream*>(png_get_io_ptr(png));
        stream.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(len));
    }

    static void PNGCBAPI writeBytes(png_struct* png, png_byte* data, size_t len)
    {
        auto& stream = *reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
        stream.write(reinterpret_cast<const char*>(data), static_cast<std::streampos>(len));
    }

    static void PNGCBAPI flush(png_struct* png)
    {
        auto& stream = *reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
        stream.flush();
    }
};

const ImageCodec& ImageCodec::getPngCodec()
{
    static const PngImageCodec instance;
    return instance;
}

}