#ifndef IMAGEAPPROVALS_IMAGEVIEW_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGEVIEW_HPP_INCLUDED

#include "ColorSpace.hpp"
#include "PixelFormat.hpp"
#include <cstdint>

namespace ImageApprovals {

struct Size
{
    uint32_t width = 0;
    uint32_t height = 0;

    bool operator ==(const Size& rhs) const
    { return (width == rhs.width) && (height == rhs.height); }

    bool operator !=(const Size& rhs) const
    { return !(*this == rhs); }
};

std::ostream& operator <<(std::ostream& stream, const Size& size);

class ImageView
{
public:
    ImageView() = default;
    ImageView(const ImageView&) = default;

    ImageView(PixelFormat format, const ColorSpace& colorSpace,
              const Size& size, size_t rowStride, const uint8_t* data);

    ImageView& operator =(const ImageView&) = default;

    PixelFormat getPixelFormat() const { return m_pixelFormat; }
    const ColorSpace& getColorSpace() const { return *m_colorSpace; }
    Size getSize() const { return m_size; }
    size_t getRowStride() const { return m_rowStride; }

    const uint8_t* getPixelData() const { return m_data; }

    const uint8_t* getRowPointer(uint32_t index) const;

    RGBA getPixel(uint32_t x, uint32_t y) const;

private:
    PixelFormat m_pixelFormat;
    const ColorSpace* m_colorSpace = &ColorSpace::getLinear();
    Size m_size;
    size_t m_rowStride = 0;
    const uint8_t* m_data = nullptr;
};

}

#endif // IMAGEAPPROVALS_IMAGEVIEW_HPP_INCLUDED
