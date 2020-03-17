#ifndef IMAGEAPPROVALS_IMAGECODEC_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGECODEC_HPP_INCLUDED

#include "Image.hpp"
#include <string>
#include <memory>
#include <vector>

namespace ImageApprovals {

class ImageCodec
{
public:
    class Disposer;

    virtual ~ImageCodec() = default;

    virtual std::string getFileExtensionWithDot() const = 0;

    virtual int getScore(const std::string& extensionWithDot) const = 0;
    virtual int getScore(const PixelFormat& pf, const ColorSpace& cs) const = 0;

    Image read(const std::string& fileName) const;
    void write(const std::string& fileName, const ImageView& image) const;

    static Disposer registerCodec(const std::shared_ptr<ImageCodec>& codec);
    static void unregisterCodec(const std::shared_ptr<ImageCodec>& codec);

    static std::vector<std::string> getRegisteredExtensions();

    static const ImageCodec& getBestCodec(const ImageView& image);
    static const ImageCodec& getBestCodec(const std::string& filePath);

protected:
    virtual Image readFromStream(std::istream& stream, const std::string& fileName) const = 0;
    virtual void writeToStream(const ImageView& image, std::ostream& stream, const std::string& fileName) const = 0;

private:
    static std::vector<std::shared_ptr<ImageCodec>>& getImageCodecs();
};

class ImageCodec::Disposer
{
public:
    explicit Disposer(std::shared_ptr<ImageCodec> codec);
    Disposer(const Disposer&) = delete;
    Disposer(Disposer&&) noexcept;
    ~Disposer();

    Disposer& operator =(const Disposer&) = delete;
    Disposer& operator =(Disposer&&) noexcept;

private:
    std::shared_ptr<ImageCodec> m_codec;
};

}

#endif // IMAGEAPPROVALS_IMAGECODEC_HPP_INCLUDED
