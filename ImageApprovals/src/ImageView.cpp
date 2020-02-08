#include <ImageApprovals/ImageView.hpp>
#include <stdexcept>
#include <ostream>

namespace ImageApprovals {

std::ostream& operator <<(std::ostream& stream, const Size& size)
{
    stream << "[" << size.width << ", " << size.height << "]";
    return stream;
}

ImageView::ImageView(const PixelFormat& format, const ColorSpace& colorSpace,
                     const Size& size, size_t rowStride, const uint8_t* data)
    : m_format(&format), m_colorSpace(&colorSpace), m_size(size),
      m_rowStride(rowStride), m_dataPtr(data)
{}

bool ImageView::isEmpty() const
{
    return m_format == nullptr;
}

const PixelFormat& ImageView::getPixelFormat() const
{
    if (!m_format)
    {
        throw std::logic_error("calling getPixelFormat on an empty ImageView");
    }

    return *m_format;
}

const ColorSpace& ImageView::getColorSpace() const
{
    if (!m_colorSpace)
    {
        throw std::logic_error("calling getColorSpace on an empty ImageView");
    }

    return *m_colorSpace;
}

const uint8_t* ImageView::getRowPointer(uint32_t index) const
{
    if (index >= m_size.height)
    {
        throw std::out_of_range("row index out of range");
    }

    return m_dataPtr + m_rowStride * index;
}

RGBA ImageView::getPixel(uint32_t x, uint32_t y) const
{
    if (x >= m_size.width)
    {
        throw std::out_of_range("x out of range");
    }

    const auto rowPtr = getRowPointer(y);

    const auto& fmt = getPixelFormat();
    const auto pixelStride = fmt.getPixelStride();

    RGBA value;
    fmt.decode(rowPtr + pixelStride * x, rowPtr + m_rowStride, value);
    return value;
}

}