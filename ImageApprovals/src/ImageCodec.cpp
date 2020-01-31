#include <ImageApprovals/ImageCodec.hpp>
#include <fstream>

namespace ImageApprovals {

Image ImageCodec::read(const std::string& fileName)
{
    std::ifstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw std::runtime_error("could not open file \"" + fileName + "\" for reading");
    }

    fileStream.exceptions(std::ios::failbit | std::ios::badbit);

    const ImageCodec* codec = &getPngCodec();
    if (codec->canRead(fileStream, fileName))
    {
        return codec->read(fileStream, fileName);
    }
    
    codec = &getExrCodec();
    if (codec->canRead(fileStream, fileName))
    {
        return codec->read(fileStream, fileName);
    }

    throw std::runtime_error("unsupported image file format when reading \"" + fileName + "\"");
}

namespace {
bool endsWith(const std::string& str, const std::string& suffix)
{
    if (str.size() < suffix.size())
    {
        return false;
    }

    return (str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
}
}

void ImageCodec::write(const std::string& fileName, const ImageView& image)
{
    const ImageCodec* codec = nullptr;

    if (endsWith(fileName, getPngCodec().getFileExtensionWithDot()))
    {
        codec = &getPngCodec();
    }
    else if (endsWith(fileName, getExrCodec().getFileExtensionWithDot()))
    {
        codec = &getExrCodec();
    }

    if (!codec)
    {
        throw std::runtime_error("unsupported image file format when writing \"" + fileName + "\"");
    }

    std::ofstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw std::runtime_error("could not open file \"" + fileName + "\" for writing");
    }

    fileStream.exceptions(std::ios::badbit | std::ios::failbit);
    codec->write(image, fileStream, fileName);
}

}