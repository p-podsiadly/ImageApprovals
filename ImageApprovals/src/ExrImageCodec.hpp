#ifndef IMAGEAPPROVALS_EXRIMAGECODEC_HPP_INCLUDED
#define IMAGEAPPROVALS_EXRIMAGECODEC_HPP_INCLUDED

#ifdef ImageApprovals_CONFIG_WITH_OPENEXR

#include <ImageApprovals/ImageCodec.hpp>

namespace ImageApprovals { namespace detail {

class ExrImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override;

    int getScore(const std::string& extensionWithDot) const override;
    int getScore(const PixelFormat& pf, const ColorSpace& cs) const override;

protected:
    Image read(std::istream& stream, const std::string& fileName) const override;
    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override;
};

} }

#endif ImageApprovals_CONFIG_WITH_OPENEXR

#endif // IMAGEAPPROVALS_EXRIMAGECODEC_HPP_INCLUDED
