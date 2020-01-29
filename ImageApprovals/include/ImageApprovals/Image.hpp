#ifndef IMAGEAPPROVALS_IMAGE_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGE_HPP_INCLUDED

#include "ImageView.hpp"
#include <memory>

namespace ImageApprovals {

class Image
{
public:
    Image() = default;
    Image(const Image&) = delete;
    Image(Image&& other) noexcept;
    Image(PixelFormat format, const ColorSpace& colorSpace, const Size& size, size_t rowAlignment = 4);
    ~Image() noexcept;

    Image& operator =(const Image&) = delete;
    Image& operator =(Image&& rhs) noexcept;

    PixelFormat getPixelFormat() const { return m_format; }
    const ColorSpace& getColorSpace() const { return *m_colorSpace; }
    Size getSize() const { return m_size; }
    size_t getRowAlignment() const { return m_rowAlignment; }
    size_t getRowStride() const;

    uint8_t* getPixelData() { return m_data.get(); }
    const uint8_t* getPixelData() const { return m_data.get(); }

    uint8_t* getRowPointer(uint32_t y);
    const uint8_t* getRowPointer(uint32_t y) const;

    void flipVertically();

    ImageView getView() const;

private:
    PixelFormat m_format;
    const ColorSpace* m_colorSpace = &ColorSpace::getLinear();
    Size m_size;
    size_t m_rowAlignment = 0;
    std::unique_ptr<uint8_t[]> m_data;
};

}

#endif // IMAGEAPPROVALS_IMAGE_HPP_INCLUDED