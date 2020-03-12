#include <ImageApprovals/ImageCodec.hpp>
#include <ImageApprovals/Errors.hpp>
#include <ApprovalTests.hpp>
#include <algorithm>
#include <fstream>
#include <vector>

#include "ExrImageCodec.hpp"
#include "PngImageCodec.hpp"

namespace ImageApprovals {

using ApprovalTests::StringUtils;

ImageCodec::Disposer ImageCodec::registerCodec(const std::shared_ptr<ImageCodec>& codec)
{
    if (codec)
    {
        getImageCodecs().insert(getImageCodecs().begin(), codec);
    }

    return Disposer(codec);
}

void ImageCodec::unregisterCodec(const std::shared_ptr<ImageCodec>& codec)
{
    if (!codec)
    {
        return;
    }

    auto& codecs = getImageCodecs();

    auto pos = std::find(codecs.begin(), codecs.end(), codec);
    if (pos != codecs.end())
    {
        codecs.erase(pos);
    }
}

std::vector<std::string> ImageCodec::getRegisteredExtensions()
{
    auto& codecs = getImageCodecs();

    std::vector<std::string> extensions;
    extensions.resize(codecs.size());

    transform(codecs.begin(), codecs.end(), extensions.begin(),
        [](const std::shared_ptr<ImageCodec>& codec) { return codec->getFileExtensionWithDot(); });

    return extensions;
}

Image ImageCodec::read(const std::string& fileName)
{
    std::ifstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for reading");
    }

    fileStream.exceptions(std::ios::failbit | std::ios::badbit);

    for (const auto& codec : getImageCodecs())
    {
        const bool canRead = codec->canRead(fileStream, fileName);
        fileStream.seekg(0);

        if(canRead)
        {            
            return codec->read(fileStream, fileName);
        }
    }

    throw ImageApprovalsError("Unsupported image file format when reading \"" + fileName + "\"");
}

void ImageCodec::write(const std::string& fileName, const ImageView& image)
{
    const ImageCodec* matchingCodec = nullptr;

    for (const auto& codec : getImageCodecs())
    {
        if (StringUtils::endsWith(fileName, codec->getFileExtensionWithDot()))
        {
            matchingCodec = codec.get();
            break;
        }
    }

    if (!matchingCodec)
    {
        throw ImageApprovalsError("Unsupported image file format when writing \"" + fileName + "\"");
    }

    std::ofstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for writing");
    }

    fileStream.exceptions(std::ios::badbit | std::ios::failbit);
    matchingCodec->write(image, fileStream, fileName);
}

std::vector<std::shared_ptr<ImageCodec>>& ImageCodec::getImageCodecs()
{
    static std::vector<std::shared_ptr<ImageCodec>> imageCodecs = {
        std::make_shared<ExrImageCodec>(),
        std::make_shared<PngImageCodec>()
    };

    return imageCodecs;
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