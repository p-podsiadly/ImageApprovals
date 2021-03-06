#ifdef ImageApprovals_CONFIG_WITH_LIBPNG

#include "PngImageCodec.hpp"
#include "ColorSpaceUtils.hpp"
#include <ImageApprovals/Errors.hpp>
#include <png.h>
#include <functional>
#include <cstring>
#include <istream>
#include <ostream>

namespace ImageApprovals { namespace detail {

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

void writePngComment(png_struct* png, png_info* info)
{
    std::string textKey = "Software";
    std::string textStr = "ImageApprovals";

    png_text text;
    text.compression = PNG_TEXT_COMPRESSION_NONE;
    text.key = &textKey[0];
    text.text = &textStr[0];
    text.text_length = textStr.size();
    text.itxt_length = 0;
    text.lang = nullptr;
    text.lang_key = nullptr;

    png_set_text(png, info, &text, 1);
}

}

std::string PngImageCodec::getFileExtensionWithDot() const
{
    return ".png";
}

int PngImageCodec::getScore(const std::string& extensionWithDot) const
{
    if(extensionWithDot == ".png")
    {
        return 100;
    }

    return -1;
}

int PngImageCodec::getScore(const PixelFormat& pf, const ColorSpace& cs) const
{
    if(!pf.isU8())
    {
        return -1;
    }

    if(cs != ColorSpace::getSRgb() && cs != ColorSpace::getLinearSRgb())
    {
        return -1;
    }

    return 100;
}

Image PngImageCodec::readFromStream(std::istream& stream, const std::string&) const
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
        throw ImageApprovalsError("Failed to allocate PNG read struct");
    }
        
    if (!(info = png_create_info_struct(png)))
    {
        throw ImageApprovalsError("Failed to allocate PNG info struct");
    }

    if (setjmp(png_jmpbuf(png)))
    {
        throw ImageApprovalsError("Failed to read PNG image");
    }

    png_set_read_fn(png, &stream, &readBytes);

    png_read_png(png, info, 0, nullptr);

    const PixelFormat* format = nullptr;
    const Size imgSize{ png_get_image_width(png, info), png_get_image_height(png, info) };

    const int pngBitDepth = png_get_bit_depth(png, info);
    const int pngColorType = png_get_color_type(png, info);

    if (pngBitDepth != 8)
    {
        throw ImageApprovalsError("Unsupported PNG bit depth");
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
        throw ImageApprovalsError("Unsupported PNG color type");
    }

    const ColorSpace* colorSpace = detectColorSpace(png, info);
    if (!colorSpace)
    {
        throw ImageApprovalsError("Unknown color space");
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

void PngImageCodec::writeToStream(const ImageView& image, std::ostream& stream, const std::string&) const
{
    const auto& fmt = image.getPixelFormat();

    if (!fmt.isU8())
    {
        throw ImageApprovalsError("Unable to write the image to PNG file");
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
        throw ImageApprovalsError("Failed to allocate PNG write struct");
    }

    if (!(info = png_create_info_struct(png)))
    {
        throw ImageApprovalsError("Failed to allocate PNG info struct");
    }

    if (setjmp(png_jmpbuf(png)))
    {
        throw ImageApprovalsError("Failed to write PNG image");
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
        throw ImageApprovalsError("Unexpected pixel format");
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
    else if (image.getColorSpace() == ColorSpace::getLinearSRgb())
    {
        png_set_gAMA(png, info, 1.0);

        const auto p = RgbPrimaries::getSRgbPrimaries();
        png_set_cHRM(
            png, info,
            0.3127, 0.3291,
            p.r.x, p.r.y,
            p.g.x, p.g.y,
            p.b.x, p.b.y);
    }
    else
    {
        throw ImageApprovalsError("Unsupported ColorSpace");
    }

    writePngComment(png, info);

    rowPointers.reset(new png_const_bytep[sz.height]);
    for (uint32_t y = 0; y < sz.height; ++y)
    {
        rowPointers[y] = reinterpret_cast<const png_byte*>(image.getRowPointer(y));
    }

    png_set_rows(png, info, const_cast<png_bytepp>(rowPointers.get()));

    png_write_png(png, info, 0, nullptr);
}

} }

#endif // ImageApprovals_CONFIG_WITH_LIBPNG