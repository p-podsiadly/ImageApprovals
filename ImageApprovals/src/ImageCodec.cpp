#include <ImageApprovals/ImageCodec.hpp>
#include <ApprovalTests.hpp>
#include <algorithm>
#include <fstream>
#include <vector>

#include "ExrImageCodec.hpp"
#include "PngImageCodec.hpp"

namespace ImageApprovals {

using ApprovalTests::StringUtils;

namespace {

std::vector<std::shared_ptr<ImageCodec>> imageCodecs = {
    std::make_shared<ExrImageCodec>(),
    std::make_shared<PngImageCodec>()
};

}

ImageCodec::Disposer ImageCodec::registerCodec(const std::shared_ptr<ImageCodec>& codec)
{
    if (codec)
    {
        imageCodecs.insert(imageCodecs.begin(), codec);
    }

    return Disposer(codec);
}

void ImageCodec::unregisterCodec(const std::shared_ptr<ImageCodec>& codec)
{
    if (!codec)
    {
        return;
    }

    auto pos = std::find(imageCodecs.begin(), imageCodecs.end(), codec);
    if (pos != imageCodecs.end())
    {
        imageCodecs.erase(pos);
    }
}

std::vector<std::string> ImageCodec::getRegisteredExtensions()
{
    std::vector<std::string> extensions;
    extensions.resize(imageCodecs.size());

    transform(imageCodecs.begin(), imageCodecs.end(), extensions.begin(),
        [](const std::shared_ptr<ImageCodec>& codec) { return codec->getFileExtensionWithDot(); });

    return extensions;
}

Image ImageCodec::read(const std::string& fileName)
{
    std::ifstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw std::runtime_error("could not open file \"" + fileName + "\" for reading");
    }

    fileStream.exceptions(std::ios::failbit | std::ios::badbit);

    for (const auto& codec : imageCodecs)
    {
        const bool canRead = codec->canRead(fileStream, fileName);
        fileStream.seekg(0);

        if(canRead)
        {            
            return codec->read(fileStream, fileName);
        }
    }

    throw std::runtime_error("unsupported image file format when reading \"" + fileName + "\"");
}

void ImageCodec::write(const std::string& fileName, const ImageView& image)
{
    const ImageCodec* matchingCodec = nullptr;

    for (const auto& codec : imageCodecs)
    {
        if (StringUtils::endsWith(fileName, codec->getFileExtensionWithDot()))
        {
            matchingCodec = codec.get();
            break;
        }
    }

    if (!matchingCodec)
    {
        throw std::runtime_error("unsupported image file format when writing \"" + fileName + "\"");
    }

    std::ofstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw std::runtime_error("could not open file \"" + fileName + "\" for writing");
    }

    fileStream.exceptions(std::ios::badbit | std::ios::failbit);
    matchingCodec->write(image, fileStream, fileName);
}

ImageCodec::Disposer::Disposer(std::shared_ptr<ImageCodec> codec)
    : m_codec(std::move(codec))
{}

ImageCodec::Disposer::Disposer(Disposer&& other) noexcept
    : m_codec(std::move(other.m_codec))
{}

ImageCodec::Disposer::~Disposer()
{
    if (m_codec)
    {
        ImageCodec::unregisterCodec(m_codec);
    }
}

ImageCodec::Disposer& ImageCodec::Disposer::operator =(Disposer&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_codec = std::move(rhs.m_codec);
    }

    return *this;
}

}