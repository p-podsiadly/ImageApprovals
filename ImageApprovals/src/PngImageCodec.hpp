#ifndef IMAGEAPPROVALS_PNGIMAGECODEC_HPP_INCLUDED
#define IMAGEAPPROVALS_PNGIMAGECODEC_HPP_INCLUDED

#include <ImageApprovals/ImageCodec.hpp>

namespace ImageApprovals {

class PngImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override;

    int getScore(const std::string& extensionWithDot) const override;
    int getScore(const std::string& extensionWithDot, const PixelFormat& pf, const ColorSpace& cs) const override;

    Image read(std::istream& stream, const std::string& fileName) const override;
    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override;
};

}

#endif // IMAGEAPPROVALS_PNGIMAGECODEC_HPP_INCLUDED
