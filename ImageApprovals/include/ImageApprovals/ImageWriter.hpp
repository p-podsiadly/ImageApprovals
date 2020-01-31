#ifndef IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED

#include "ImageCodec.hpp"
#include "Image.hpp"
#include <ApprovalTests.hpp>
#include <stdexcept>

namespace ImageApprovals {

enum class Format
{
    PNG,
    EXR
};

class ImageWriter : public ApprovalTests::ApprovalWriter
{
public:
    ImageWriter(const ImageView& image, Format format)
        : m_image(image), m_format(format)
    {}

    ImageWriter(const Image& image, Format format)
        : ImageWriter(image.getView(), format)
    {}

    ImageWriter(const ImageWriter&) = delete;

    std::string getFileExtensionWithDot() const override
    {
        switch (m_format)
        {
        case Format::PNG:
            return ".png";
        case Format::EXR:
            return ".exr";
        }

        throw std::runtime_error("invalid Format value");
    }

    void write(std::string path) const override
    {
        ImageCodec::write(path, m_image);
    }

    void cleanUpReceived(std::string receivedPath) const override
    {
        remove(receivedPath.c_str());
    }

private:
    const ImageView m_image;
    Format m_format;
};

}

#endif // IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED
