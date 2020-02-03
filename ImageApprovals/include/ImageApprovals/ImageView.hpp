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

    constexpr Size() = default;
    constexpr Size(const Size&) = default;
    
    constexpr Size(uint32_t width, uint32_t height)
        : width(width), height(height)
    {}

    Size& operator =(const Size&) = default;

    bool operator ==(const Size& rhs) const
    { return (width == rhs.width) && (height == rhs.height); }

    bool operator !=(const Size& rhs) const
    { return !(*this == rhs); }

    bool isZero() const
    { return (width == 0) || (height == 0); }
};

std::ostream& operator <<(std::ostream& stream, const Size& size);

class ImageView
{
public:
    ImageView() = default;
    ImageView(const ImageView&) = default;

    ImageView(const PixelFormat& format, const ColorSpace& colorSpace,
              const Size& size, size_t rowStride, const uint8_t* data);

    ImageView& operator =(const ImageView&) = default;

    bool isEmpty() const;

    const PixelFormat& getPixelFormat() const;
    const ColorSpace& getColorSpace() const;

    Size getSize() const { return m_size; }
    size_t getRowStride() const { return m_rowStride; }

    const uint8_t* getPixelData() const { return m_data; }

    const uint8_t* getRowPointer(uint32_t index) const;

    RGBA getPixel(uint32_t x, uint32_t y) const;

private:
    const PixelFormat* m_pixelFormat = nullptr;
    const ColorSpace* m_colorSpace = nullptr;
    Size m_size;
    size_t m_rowStride = 0;
    const uint8_t* m_data = nullptr;
};

}

#endif // IMAGEAPPROVALS_IMAGEVIEW_HPP_INCLUDED
