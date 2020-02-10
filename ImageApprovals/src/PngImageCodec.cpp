#include "PngImageCodec.hpp"
#include "ColorSpaceUtils.hpp"
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

void PNGCBAPI readBytes(png_struct* png, png_byte* data, size_t len)
{
    auto& stream = *reinterpret_cast<std::istream*>(png_get_io_ptr(png));
    stream.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(len));
}

void PNGCBAPI writeBytes(png_struct* png, png_byte* data, size_t len)
{
    auto& stream = *reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
    stream.write(reinterpret_cast<const char*>(data), static_cast<std::streampos>(len));
}

void PNGCBAPI flush(png_struct* png)
{
    auto& stream = *reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
    stream.flush();
}

const ColorSpace* detectColorSpace(png_struct* png, png_info* info)
{
    int intent = -1;
    if (png_get_sRGB(png, info, &intent) == PNG_INFO_sRGB)
    {
        return &ColorSpace::getSRgb();
    }

    char* iccpName = nullptr;
    int iccpCompressionType = PNG_COMPRESSION_TYPE_BASE;
    png_byte* iccpData = nullptr;
    uint32_t iccpDataLen = 0;
    const auto iCCP
        = png_get_iCCP(
            png, info,
            &iccpName,
            &iccpCompressionType,
            &iccpData, &iccpDataLen);

    if (iCCP == PNG_INFO_iCCP && isSRgbIccProfile(iccpDataLen, iccpData))
    {
        return &ColorSpace::getSRgb();
    }

    double white_x = 0.0, white_y = 0.0;
    RgbPrimaries prims;

    const auto cHRM
        = png_get_cHRM(
            png, info,
            &white_x, &white_y,
            &prims.r.x, &prims.r.y,
            &prims.g.x, &prims.g.y,
            &prims.b.x, &prims.b.y);

    double gamma = 0.0f;
    const auto gAMA = png_get_gAMA(png, info, &gamma);

    if (cHRM == PNG_INFO_cHRM && gAMA == PNG_INFO_gAMA)
    {
        return detectColorSpace(prims, gamma);
    }

    return &ColorSpace::getLinearSRgb();
}

}

std::string PngImageCodec::getFileExtensionWithDot() const
{
    return ".png";
}

bool PngImageCodec::canRead(std::istream& stream, const std::string&) const
{
    png_byte signature[8];
    std::fill_n(signature, 8, 0);

    stream.read(reinterpret_cast<char*>(signature), 8);
    stream.seekg(0);

    return (png_sig_cmp(signature, 0, 8) == 0);
}

Image PngImageCodec::read(std::istream& stream, const std::string&) const
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

    png_set_read_fn(png, &stream, &readBytes);

    png_read_png(png, info, 0, nullptr);

    const PixelFormat* format = nullptr;
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
        format = &PixelFormat::getGrayU8();
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        format = &PixelFormat::getGrayAlphaU8();
        break;
    case PNG_COLOR_TYPE_RGB:
        format = &PixelFormat::getRgbU8();
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        format = &PixelFormat::getRgbAlphaU8();
        break;
    default:
        throw std::runtime_error("unsupported PNG color type");
    }

    const ColorSpace* colorSpace = detectColorSpace(png, info);
    if (!colorSpace)
    {
        throw std::runtime_error("unknown color space");
    }

    image = Image(*format, *colorSpace, imgSize);

    png_byte** rowPointers = png_get_rows(png, info);
    const size_t rowSize = format->getPixelStride() * imgSize.width;
    for (uint32_t y = 0; y < imgSize.height; ++y)
    {
        const png_byte* srcRow = rowPointers[y];
        uint8_t* dstRow = image.getRowPointer(y);
        std::memcpy(dstRow, srcRow, rowSize);
    }

    return image;
}

void PngImageCodec::write(const ImageView& image, std::ostream& stream, const std::string&) const
{
    const auto& fmt = image.getPixelFormat();

    if (!fmt.isU8())
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

    png_set_write_fn(png, &stream, &writeBytes, &flush);

    int pngColorType = 0;
    if (fmt == PixelFormat::getGrayU8())
    {
        pngColorType = PNG_COLOR_TYPE_GRAY;
    }
    else if (fmt == PixelFormat::getGrayAlphaU8())
    {
        pngColorType = PNG_COLOR_TYPE_GRAY_ALPHA;
    }
    else if (fmt == PixelFormat::getRgbU8())
    {
        pngColorType = PNG_COLOR_TYPE_RGB;
    }
    else if (fmt == PixelFormat::getRgbAlphaU8())
    {
        pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
    }
    else
    {
        throw std::runtime_error("unexpected pixel format");
    }

    const auto sz = image.getSize();
    png_set_IHDR(
        png, info, sz.width, sz.height,
        8, pngColorType,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    if (image.getColorSpace() == ColorSpace::getSRgb())
    {
        png_set_sRGB_gAMA_and_cHRM(png, info, PNG_sRGB_INTENT_RELATIVE);
    }
    else if (image.getColorSpace() != ColorSpace::getLinearSRgb())
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

}