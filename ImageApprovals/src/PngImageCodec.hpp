#ifndef IMAGEAPPROVALS_PNGIMAGECODEC_HPP_INCLUDED
#define IMAGEAPPROVALS_PNGIMAGECODEC_HPP_INCLUDED

#include <ImageApprovals/ImageCodec.hpp>

namespace ImageApprovals {

class PngImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override;

    bool canRead(std::istream& stream, const std::string& fileName) const override;
    Image read(std::istream& stream, const std::string& fileName) const override;

    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override;
};

}

#endif // IMAGEAPPROVALS_PNGIMAGECODEC_HPP_INCLUDED
