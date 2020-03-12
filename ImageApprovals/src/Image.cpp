#include <ImageApprovals/Image.hpp>
#include <ImageApprovals/Errors.hpp>
#include <cstring>
#include <fstream>

namespace ImageApprovals {

namespace {

size_t alignedSize(size_t baseSize, size_t alignment)
{
    return ((baseSize + alignment - 1) / alignment) * alignment;
}

size_t rowStride(const PixelFormat& fmt, const Size& sz, size_t rowAlignment)
{
    if (rowAlignment == 0)
    {
        return 0;
    }

    const auto rowSize = fmt.getPixelStride() * sz.width;
    return alignedSize(rowSize, rowAlignment);
}

}

Image::Image(Image&& other) noexcept
{
    *this = std::move(other);
}

Image::Image(const PixelFormat& format, const ColorSpace& colorSpace, const Size& size, size_t rowAlignment)
    : ImageView(format, colorSpace, size, rowStride(format, size, rowAlignment), nullptr), m_rowAlignment(rowAlignment)
{
    if (m_size.isZero())
    {
        throw ImageApprovalsError("Image size cannot be zero");
    }

    if (m_rowAlignment == 0)
    {
        throw ImageApprovalsError("Image row alignment must be greater than 0");
    }

    const size_t rowStride = getRowStride();

    m_data.reset(new uint8_t[rowStride * m_size.height]);
    m_dataPtr = m_data.get();

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

        m_rowStride = rhs.m_rowStride;
        rhs.m_rowStride = 0;

        m_rowAlignment = rhs.m_rowAlignment;
        rhs.m_rowAlignment = 0;

        m_data = std::move(rhs.m_data);

        m_dataPtr = m_data.get();
        rhs.m_dataPtr = nullptr;
    }

    return *this;
}

uint8_t* Image::getRowPointer(uint32_t y)
{
    if (y >= m_size.height)
    {
        throw ImageApprovalsError("Row index out of range");
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

}