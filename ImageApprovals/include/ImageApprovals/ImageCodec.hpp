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

    static Disposer registerCodec(const std::shared_ptr<ImageCodec>& codec);
    static void unregisterCodec(const std::shared_ptr<ImageCodec>& codec);

    static std::vector<std::string> getRegisteredExtensions();

    static Image read(const std::string& fileName);
    static void write(const std::string& fileName, const ImageView& image);

protected:
    virtual std::string getFileExtensionWithDot() const = 0;

    virtual bool canRead(std::istream& stream, const std::string& fileName) const = 0;
    virtual Image read(std::istream& stream, const std::string& fileName) const = 0;

    virtual void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const = 0;
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
