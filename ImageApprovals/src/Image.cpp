#include <ImageApprovals/Image.hpp>
#include <cstring>
#include <fstream>

namespace ImageApprovals {

namespace {

size_t alignedSize(size_t baseSize, size_t alignment)
{
    return ((baseSize + alignment - 1) / alignment) * alignment;
}

}

Image::Image(Image&& other) noexcept
{
    *this = std::move(other);
}

Image::Image(const PixelFormat& format, const ColorSpace& colorSpace, const Size& size, size_t rowAlignment)
    : m_format(&format), m_colorSpace(&colorSpace), m_size(size), m_rowAlignment(rowAlignment)
{
    if (m_size.isZero())
    {
        throw std::logic_error("image size cannot be zero");
    }

    if (m_rowAlignment == 0)
    {
        throw std::logic_error("image row alignment must be greater than 0");
    }

    const size_t rowStride = getRowStride();

    m_data.reset(new uint8_t[rowStride * m_size.height]);

    for (uint32_t y = 0; y < m_size.height; ++y)
    {
        std::memset(getRowPointer(y), 0, rowStride);
    }
}

Image::~Image() noexcept = default;

Image& Image::operator =(Image&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_format = rhs.m_format;
        rhs.m_format = nullptr;

        m_colorSpace = rhs.m_colorSpace;
        rhs.m_colorSpace = nullptr;

        m_size = rhs.m_size;
        rhs.m_size = {};

        m_rowAlignment = rhs.m_rowAlignment;
        rhs.m_rowAlignment = 0;

        m_data = std::move(rhs.m_data);
    }

    return *this;
}

bool Image::isEmpty() const
{
    return m_format == nullptr;
}

const PixelFormat& Image::getPixelFormat() const
{
    if (!m_format)
    {
        throw std::logic_error("call to getPixelFormat on an empty image");
    }

    return *m_format;
}

const ColorSpace& Image::getColorSpace() const
{
    if (!m_colorSpace)
    {
        throw std::logic_error("call to getColorSpace on an empty image");
    }

    return *m_colorSpace;
}

size_t Image::getRowStride() const
{
    const auto pixelStride = getPixelFormat().getPixelStride();
    return alignedSize(pixelStride * m_size.width, m_rowAlignment);
}

uint8_t* Image::getRowPointer(uint32_t y)
{
    if (y >= m_size.height)
    {
        throw std::out_of_range("row index out of range");
    }

    const auto rowStride = getRowStride();
    return &m_data[rowStride * y];
}

const uint8_t* Image::getRowPointer(uint32_t y) const
{
    if (y >= m_size.height)
    {
        throw std::out_of_range("row index out of range");
    }

    const auto rowStride = getRowStride();
    return &m_data[rowStride * y];
}

void Image::flipVertically()
{
    const auto rowStride = getRowStride();

    std::unique_ptr<uint8_t[]> rowBuffer;
    rowBuffer.reset(new uint8_t[rowStride]);

    for (uint32_t y = 0; y < m_size.height / 2; ++y)
    {
        auto upperRow = getRowPointer(y);
        auto lowerRow = getRowPointer(m_size.height - y - 1);

        std::memcpy(rowBuffer.get(), upperRow, rowStride);
        std::memcpy(upperRow, lowerRow, rowStride);
        std::memcpy(lowerRow, rowBuffer.get(), rowStride);
    }
}

ImageView Image::getView() const
{
    return ImageView(getPixelFormat(), getColorSpace(),
                     m_size, getRowStride(), m_data.get());
}

}