#ifndef IMAGEAPPROVALS_IMAGECODEC_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGECODEC_HPP_INCLUDED

#include "Image.hpp"
#include <string>

namespace ImageApprovals {

class ImageCodec
{
public:
    virtual ~ImageCodec() = default;

    virtual std::string getFileExtensionWithDot() const = 0;

    virtual bool canRead(std::istream& stream) const = 0;
    virtual Image read(std::istream& stream) const = 0;

    virtual void write(const ImageView& image, std::ostream& stream) const = 0;

    static const ImageCodec& getPngCodec();

    static Image read(const std::string& fileName);
    static void write(const std::string& fileName, const ImageView& image);
};

}

#endif // IMAGEAPPROVALS_IMAGECODEC_HPP_INCLUDED
