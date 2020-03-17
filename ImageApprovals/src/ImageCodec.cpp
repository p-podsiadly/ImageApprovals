#include <ImageApprovals/ImageCodec.hpp>
#include <ImageApprovals/Errors.hpp>
#include <ApprovalTests.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include "ExrImageCodec.hpp"
#include "PngImageCodec.hpp"

namespace ImageApprovals {

namespace detail {

std::vector<std::shared_ptr<ImageCodec>> initCodecs()
{
    std::vector<std::shared_ptr<ImageCodec>> codecs;

#ifdef ImageApprovals_CONFIG_WITH_LIBPNG
    codecs.push_back(std::make_shared<PngImageCodec>());
#endif // ImageApprovals_CONFIG_WITH_LIBPNG

#ifdef ImageApprovals_CONFIG_WITH_OPENEXR
    codecs.push_back(std::make_shared<ExrImageCodec>());
#endif // ImageApprovals_CONFIG_WITH_OPENEXR

    return codecs;
}

template<typename... ArgTypes>
const ImageCodec* findBestMatch(const std::vector<std::shared_ptr<ImageCodec>>& codecs, const ArgTypes&... args)
{
    const ImageCodec* bestCodec = nullptr;
    int bestScore = -1;

    for(auto iter = codecs.rbegin(); iter != codecs.rend(); ++iter)
    {
        const ImageCodec* codec = iter->get();
        const int score = codec->getScore(args...);

        if((score >= 0) && (score > bestScore))
        {
            bestCodec = codec;
            bestScore = score;
        }
    }

    return bestCodec;
}

}

Image ImageCodec::read(const std::string& fileName) const
{
    std::ifstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for reading");
    }

    fileStream.exceptions(std::ios::failbit | std::ios::badbit);

    return readFromStream(fileStream, fileName);
}

void ImageCodec::write(const std::string& fileName, const ImageView& image) const
{
    std::ofstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for writing");
    }

    fileStream.exceptions(std::ios::badbit | std::ios::failbit);

    writeToStream(image, fileStream, fileName);
}

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

const ImageCodec& ImageCodec::getBestCodec(const ImageView& image)
{
    const auto& pf = image.getPixelFormat();
    const auto& cs = image.getColorSpace();

    const ImageCodec* codec = detail::findBestMatch(getImageCodecs(), pf, cs);

    if(!codec)
    {
        std::ostringstream msg;
        msg << "Could find a codec for pixel format " << pf << " and color space " << cs;
        throw ImageApprovalsError(msg.str());
    }

    return *codec;
}

const ImageCodec& ImageCodec::getBestCodec(const std::string& filePath)
{
    using namespace ApprovalTests;
    const std::string extWithDot = FileUtils::getExtensionWithDot(filePath);

    const ImageCodec* codec = detail::findBestMatch(getImageCodecs(), extWithDot);

    if(!codec)
    {
        std::ostringstream msg;
        msg << "Could not find a codec for file extension \"" << extWithDot << "\"";
        throw ImageApprovalsError(msg.str());
    }

    return *codec;
}

std::vector<std::shared_ptr<ImageCodec>>& ImageCodec::getImageCodecs()
{
    static std::vector<std::shared_ptr<ImageCodec>> imageCodecs = detail::initCodecs();
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