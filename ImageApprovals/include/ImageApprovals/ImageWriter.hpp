#ifndef IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED

#include "ImageCodec.hpp"
#include "Image.hpp"
#include "Errors.hpp"
#include <ApprovalTests.hpp>
#include <stdexcept>
#include <type_traits>

namespace ImageApprovals {

namespace detail {

template<typename T>
std::true_type supportsMakeViewMeta(T*, ImageView = makeView(*(const T*)nullptr));
std::false_type supportsMakeViewMeta(...);

template<typename T>
constexpr bool supportsMakeView()
{
    using namespace ImageApprovals;
    return decltype(supportsMakeViewMeta((T*)nullptr))::value;
}

}

template<typename T>
struct SupportsMakeView : std::integral_constant<bool, detail::supportsMakeView<T>()> {};

enum class Format
{
    Auto,
    PNG,
    EXR
};

class ImageWriter : public ApprovalTests::ApprovalWriter
{
public:
    template<typename T, typename = typename std::enable_if<SupportsMakeView<T>::value, void>::type>
    explicit ImageWriter(const T& image, Format format = Format::Auto)
        : m_image(makeView(image)), m_format(format)
    {}

    explicit ImageWriter(const ImageView& image, Format format = Format::Auto)
        : m_image(image), m_format(format)
    {}

    ImageWriter(const ImageWriter&) = delete;

    std::string getFileExtensionWithDot() const override
    {
        switch (m_format)
        {
        case Format::Auto:
            return autoDetectExtension();
        case Format::PNG:
            return ".png";
        case Format::EXR:
            return ".exr";
        }

        throw ImageApprovalsError("Invalid Format value");
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

    std::string autoDetectExtension() const
    {
        const auto& fmt = m_image.getPixelFormat();

        if (fmt.isU8())
        {
            return ".png";
        }
        else if (fmt.isF32())
        {
            return ".exr";
        }

        throw ImageApprovalsError("Invalid PixelDataType value");
    }
};

}

#endif // IMAGEAPPROVALS_IMAGEWRITER_HPP_INCLUDED
