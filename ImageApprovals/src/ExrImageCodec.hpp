#ifndef IMAGEAPPROVALS_EXRIMAGECODEC_HPP_INCLUDED
#define IMAGEAPPROVALS_EXRIMAGECODEC_HPP_INCLUDED

#include <ImageApprovals/ImageCodec.hpp>

namespace ImageApprovals {

class ExrImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override;

    bool canRead(std::istream& stream, const std::string& fileName) const override;
    Image read(std::istream& stream, const std::string& fileName) const override;

    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override;
};

}

#endif // IMAGEAPPROVALS_EXRIMAGECODEC_HPP_INCLUDED
