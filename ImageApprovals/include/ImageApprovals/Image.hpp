#ifndef IMAGEAPPROVALS_IMAGE_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGE_HPP_INCLUDED

#include "ImageView.hpp"
#include <memory>

namespace ImageApprovals {

class Image : public ImageView
{
public:
    Image() = default;
    Image(const Image&) = delete;
    Image(Image&& other) noexcept;
    Image(const PixelFormat& format, const ColorSpace& colorSpace, const Size& size, size_t rowAlignment = 4);
    ~Image() noexcept;

    Image& operator =(const Image&) = delete;
    Image& operator =(Image&& rhs) noexcept;

    size_t getRowAlignment() const { return m_rowAlignment; }

    uint8_t* getPixelData() { return m_data.get(); }
    const uint8_t* getPixelData() const { return m_data.get(); }

    uint8_t* getRowPointer(uint32_t y);

    void flipVertically();

private:
    size_t m_rowAlignment = 0;
    std::unique_ptr<uint8_t[]> m_data;
};

}

#endif // IMAGEAPPROVALS_IMAGE_HPP_INCLUDED