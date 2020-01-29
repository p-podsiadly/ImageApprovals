#include <ImageApprovals/ImageView.hpp>
#include <stdexcept>
#include <ostream>

namespace ImageApprovals {

std::ostream& operator <<(std::ostream& stream, const Size& size)
{
    stream << "[" << size.width << ", " << size.height << "]";
    return stream;
}

ImageView::ImageView(PixelFormat format, const ColorSpace& colorSpace,
                     const Size& size, size_t rowStride, const uint8_t* data)
    : m_pixelFormat(format), m_colorSpace(&colorSpace), m_size(size),
      m_rowStride(rowStride), m_data(data)
{}

const uint8_t* ImageView::getRowPointer(uint32_t index) const
{
    if (index >= m_size.height)
    {
        throw std::out_of_range("row index out of range");
    }

    return m_data + m_rowStride * index;
}

RGBA ImageView::getPixel(uint32_t x, uint32_t y) const
{
    if (x >= m_size.width)
    {
        throw std::out_of_range("x out of range");
    }

    const auto rowPtr = getRowPointer(y);
    const auto stride = getPixelStride(m_pixelFormat);
    return decode(m_pixelFormat, rowPtr + stride * x, rowPtr + m_rowStride);
}

}