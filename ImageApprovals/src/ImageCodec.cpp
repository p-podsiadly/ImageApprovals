#include <ImageApprovals/ImageCodec.hpp>
#include <ImageApprovals/Errors.hpp>
#include <ApprovalTests.hpp>
#include <algorithm>
#include <fstream>
#include <vector>

#include "ExrImageCodec.hpp"
#include "PngImageCodec.hpp"

namespace ImageApprovals {

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
    using ApprovalTests::FileUtils;
    const auto extWithDot = FileUtils::getExtensionWithDot(fileName);    

    const ImageCodec* codec = nullptr;
    int codecScore = -1;

    const auto& allCodecs = getImageCodecs();
    for(auto iter = allCodecs.rbegin(); iter != allCodecs.rend(); ++iter)
    {
        const ImageCodec* thisCodec = iter->get();
        const int thisScore = thisCodec->getScore(extWithDot);

        if((thisScore >= 0) && (codecScore < thisScore))
        {
            codec = thisCodec;
            codecScore = thisScore;
        }
    }

    if(!codec)
    {
        throw ImageApprovalsError("No suitable ImageCodec for reading \"" + fileName + "\"");
    }

    std::ifstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for reading");
    }

    fileStream.exceptions(std::ios::failbit | std::ios::badbit);

    return codec->read(fileStream, fileName);
}

void ImageCodec::write(const std::string& fileName, const ImageView& image)
{
    using ApprovalTests::FileUtils;
    const auto extWithDot = FileUtils::getExtensionWithDot(fileName);    

    const ImageCodec* codec = nullptr;
    int codecScore = -1;

    const auto& allCodecs = getImageCodecs();
    for(auto iter = allCodecs.rbegin(); iter != allCodecs.rend(); ++iter)
    {
        const ImageCodec* thisCodec = iter->get();
        const int thisScore = thisCodec->getScore(extWithDot, image.getPixelFormat(), image.getColorSpace());

        if((thisScore >= 0) && (codecScore < thisScore))
        {
            codec = thisCodec;
            codecScore = thisScore;
        }
    }

    if(!codec)
    {
        throw ImageApprovalsError("No suitable codec for writing \"" + fileName + "\"");
    }

    std::ofstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for writing");
    }

    fileStream.exceptions(std::ios::badbit | std::ios::failbit);

    codec->write(image, fileStream, fileName);
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