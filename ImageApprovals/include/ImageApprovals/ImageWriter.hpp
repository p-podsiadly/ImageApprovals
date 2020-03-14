#ifndef IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED

#include "ImageCodec.hpp"
#include "Image.hpp"
#include "Errors.hpp"
#include <ApprovalTests.hpp>
#include <stdexcept>
#include <type_traits>

namespace ImageApprovals {

class ImageWriter : public ApprovalTests::ApprovalWriter
{
public:
    explicit ImageWriter(const ImageView& image)
        : m_image(image), m_codec(ImageCodec::getBestCodec(image))
    {}

    ImageWriter(const ImageWriter&) = delete;

    std::string getFileExtensionWithDot() const override
    {
        return m_codec.getFileExtensionWithDot();
    }

    void write(std::string path) const override
    {
        m_codec.write(path, m_image);
    }

    void cleanUpReceived(std::string receivedPath) const override
    {
        remove(receivedPath.c_str());
    }

private:
    const ImageView m_image;
    const ImageCodec& m_codec;
};

}

#endif // IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED
