#ifndef IMAGEAPPROVALS_HPP_INCLUDED
#define IMAGEAPPROVALS_HPP_INCLUDED

// include/ImageApprovals/ColorSpace.hpp

#include <iosfwd>

namespace ImageApprovals {

class ColorSpace
{
public:
    virtual ~ColorSpace() = default;

    bool operator ==(const ColorSpace& rhs) const
    { return this == &rhs; }

    bool operator !=(const ColorSpace& rhs) const
    { return !(*this == rhs); }

    virtual const char* getName() const = 0;

    static const ColorSpace& getLinearSRgb();

    static const ColorSpace& getSRgb();
};

std::ostream& operator <<(std::ostream& stream, const ColorSpace& colorSpace);

}

// include/ImageApprovals/Errors.hpp

#include <exception>
#include <string>

namespace ImageApprovals {

class ImageApprovalsError : public std::exception
{
public:
    explicit ImageApprovalsError(std::string message)
        : m_message(std::move(message))
    {}

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

}

// include/ImageApprovals/PixelFormat.hpp

#include <cstdint>
#include <iosfwd>

namespace ImageApprovals {

struct RGBA
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;

    constexpr RGBA() = default;
    constexpr RGBA(const RGBA&) = default;

    constexpr RGBA(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a)
    {}

    RGBA& operator =(const RGBA&) = default;

    bool operator ==(const RGBA& rhs) const
    {
        return(r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
    }

    bool operator !=(const RGBA& rhs) const
    {
        return !(*this == rhs);
    }
};

class PixelFormat
{
public:
    virtual ~PixelFormat() = default;

    bool operator ==(const PixelFormat& rhs) const
    { return this == &rhs; }

    bool operator !=(const PixelFormat& rhs) const
    { return !(*this == rhs); }

    virtual const char* getName() const = 0;

    virtual size_t getNumberOfChannels() const = 0;
    virtual size_t getPixelStride() const = 0;
    virtual bool isU8() const = 0;
    virtual bool isF32() const = 0;

    const uint8_t* decode(const uint8_t* begin, const uint8_t* end, RGBA& outRgba) const;

    static const PixelFormat& getGrayU8();
    static const PixelFormat& getGrayAlphaU8();

    static const PixelFormat& getRgbU8();
    static const PixelFormat& getRgbAlphaU8();
    
    static const PixelFormat& getRgbF32();
    static const PixelFormat& getRgbAlphaF32();

protected:
    virtual void decode(const uint8_t* begin, RGBA& outRgba) const = 0;
};

std::ostream& operator <<(std::ostream& stream, const PixelFormat& format);
std::ostream& operator <<(std::ostream& stream, const RGBA& rgba);

}

// include/ImageApprovals/Units.hpp

#include <iosfwd>

namespace ImageApprovals {

namespace detail {

template<typename ValueType, typename Tag>
struct Unit
{
    ValueType value = ValueType(0);

    constexpr Unit() = default;
    constexpr Unit(const Unit&) = default;

    constexpr explicit Unit(ValueType value)
        : value(value)
    {}

    Unit& operator =(const Unit&) = default;

    bool operator ==(const Unit& rhs) const{ return value == rhs.value; }
    bool operator !=(const Unit& rhs) const{ return value != rhs.value; }

    bool operator <(const Unit& rhs) const{ return value < rhs.value; }
    bool operator >(const Unit& rhs) const{ return value > rhs.value; }

    bool operator <=(const Unit& rhs) const{ return value <= rhs.value; }
    bool operator >=(const Unit& rhs) const{ return value >= rhs.value; }
};

}

using AbsThreshold = detail::Unit<double, struct AbsThresholdTag>;
using Percent = detail::Unit<double, struct PercentTag>;

std::ostream& operator <<(std::ostream& stream, const AbsThreshold& threshold);
std::ostream& operator <<(std::ostream& stream, const Percent& percent);

}

// include/ImageApprovals/Version.hpp

#define ImageApprovals_VERSION_MAJOR 0
#define ImageApprovals_VERSION_MINOR 1
#define ImageApprovals_VERSION_PATCH 0

#define ImageApprovals_VERSION_STR "0.1.0"

// include/ImageApprovals/CompareStrategy.hpp

#include <string>

namespace ImageApprovals {

class ImageView;

class CompareStrategy
{
public:
    struct Result
    {
        bool passed = false;
        std::string leftImageInfo;
        std::string rightImageInfo;

        static Result makePassed();
        static Result makeFailed(std::string leftInfo, std::string rightInfo);
    };

    virtual ~CompareStrategy() = default;

    Result compare(const ImageView& left, const ImageView& right) const;

protected:
    virtual Result compareInfos(const ImageView& left, const ImageView& right) const;
    virtual Result compareContents(const ImageView& left, const ImageView& right) const = 0;
};

class ThresholdCompareStrategy : public CompareStrategy
{
public:
    explicit ThresholdCompareStrategy(
        AbsThreshold pixelFailThreshold = AbsThreshold(0.004),
        Percent maxFailedPixelsPercentage = Percent(0.1));

protected:
    Result compareContents(const ImageView& left, const ImageView& right) const override;

private:
    AbsThreshold m_pixelFailThreshold;
    Percent m_maxFailedPixelsPercentage;
};

class PixelPerfectCompareStrategy : public CompareStrategy
{
public:
    PixelPerfectCompareStrategy() = default;

protected:
    Result compareContents(const ImageView& left, const ImageView& right) const override;
};

}

// include/ImageApprovals/ImageView.hpp

#include <cstdint>

namespace ImageApprovals {

struct Size
{
    uint32_t width = 0;
    uint32_t height = 0;

    constexpr Size() = default;
    constexpr Size(const Size&) = default;
    
    constexpr Size(uint32_t width, uint32_t height)
        : width(width), height(height)
    {}

    Size& operator =(const Size&) = default;

    bool operator ==(const Size& rhs) const
    { return (width == rhs.width) && (height == rhs.height); }

    bool operator !=(const Size& rhs) const
    { return !(*this == rhs); }

    bool isZero() const
    { return (width == 0) || (height == 0); }
};

std::ostream& operator <<(std::ostream& stream, const Size& size);

class ImageView
{
public:
    ImageView() = default;
    ImageView(const ImageView&) = default;

    ImageView(const PixelFormat& format, const ColorSpace& colorSpace,
              const Size& size, size_t rowStride, const uint8_t* data);

    ImageView& operator =(const ImageView&) = default;

    bool isEmpty() const;

    const PixelFormat& getPixelFormat() const;
    const ColorSpace& getColorSpace() const;

    Size getSize() const { return m_size; }
    size_t getRowStride() const { return m_rowStride; }

    const uint8_t* getRowPointer(uint32_t index) const;

    RGBA getPixel(uint32_t x, uint32_t y) const;

protected:
    const PixelFormat* m_format = nullptr;
    const ColorSpace* m_colorSpace = nullptr;
    Size m_size;
    size_t m_rowStride = 0;
    const uint8_t* m_dataPtr = nullptr;
};

}

// include/ImageApprovals/Image.hpp

#include <memory>

namespace ImageApprovals {

class Image : public ImageView
{
public:
    Image() = default;
    Image(const Image&) = delete;
    Image(Image&& other) noexcept;
    Image(const PixelFormat& format, const ColorSpace& colorSpace, const Size& size, size_t rowAlignment = 4);
    ~Image() noexcept;

    Image& operator =(const Image&) = delete;
    Image& operator =(Image&& rhs) noexcept;

    size_t getRowAlignment() const { return m_rowAlignment; }

    uint8_t* getPixelData() { return m_data.get(); }
    const uint8_t* getPixelData() const { return m_data.get(); }

    uint8_t* getRowPointer(uint32_t y);

    void flipVertically();

private:
    size_t m_rowAlignment = 0;
    std::unique_ptr<uint8_t[]> m_data;
};

}

// include/ImageApprovals/ImageComparator.hpp

#include <ApprovalTests.hpp>

namespace ImageApprovals {

class ImageComparator : public ApprovalTests::ApprovalComparator
{
public:
    class Disposer
    {
    public:
        explicit Disposer(std::vector<ApprovalTests::ComparatorDisposer> disposers);
        Disposer(const Disposer&) = delete;
        Disposer(Disposer&&) = default;

        Disposer& operator =(const Disposer&) = delete;
        Disposer& operator =(Disposer&&) = delete;

    private:
        std::vector<ApprovalTests::ComparatorDisposer> m_disposers;
    };

    ImageComparator();
    explicit ImageComparator(std::shared_ptr<CompareStrategy> comparator);

    bool contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const override;

    template<typename ConcreteImageComparator, typename... Arguments>
    static std::shared_ptr<ImageComparator> make(Arguments&&... args)
    {
        std::shared_ptr<CompareStrategy> imgComparator(
            new ConcreteImageComparator(std::forward<Arguments>(args)...));

        return std::make_shared<ImageComparator>(std::move(imgComparator));
    }

    template<typename Strategy, typename... Arguments>
    static Disposer registerForAllExtensions(Arguments&&... args)
    {
        auto strategy = std::make_shared<Strategy>(std::forward<Arguments>(args)...);
        return registerForAllExtensions(strategy);
    }

    static Disposer registerForAllExtensions(std::shared_ptr<CompareStrategy> strategy);

private:
    std::shared_ptr<CompareStrategy> m_compareStrategy;
};

}

// include/ImageApprovals/QImageView.hpp

#ifdef ImageApprovals_CONFIG_QT5

#include <QImage>

namespace ImageApprovals {

namespace qt5 {

inline const PixelFormat* imagePixelFormat(const QImage& image)
{
    switch (image.format())
    {
        case QImage::Format_Grayscale8:
        case QImage::Format_Alpha8:
            return &PixelFormat::getGrayU8();
        case QImage::Format_RGB888:
            return &PixelFormat::getRgbU8();
        case QImage::Format_RGBA8888:
        case QImage::Format_RGBX8888:
            return &PixelFormat::getRgbAlphaU8();
        default:
            break;
    }

    return nullptr;
}

inline const ColorSpace* imageColorSpace(const QImage &image)
{
    // TODO QColorSpace was added in Qt 5.14
    ((void) image);
    return &ColorSpace::getSRgb();
}

}

inline ImageView makeView(const QImage& image)
{
    auto format = qt5::imagePixelFormat(image);
    auto colorSpace = qt5::imageColorSpace(image);

    if(!format)
    {
        throw ImageApprovalsError("Unsupported QImage::Format value");
    }

    if(!colorSpace)
    {
        throw ImageApprovalsError("Unsupported QColorSpace");
    }

    const auto w = static_cast<uint32_t>(image.width());
    const auto h = static_cast<uint32_t>(image.height());
    const auto stride = static_cast<size_t>(image.bytesPerLine());

    return ImageView(*format, *colorSpace, Size(w, h), stride, image.constBits());
}

}

#endif // ImageApprovals_CONFIG_QT5

// include/ImageApprovals/ImageCodec.hpp

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

    void write(const std::string& fileName, const ImageView& image) const;

    static Disposer registerCodec(const std::shared_ptr<ImageCodec>& codec);
    static void unregisterCodec(const std::shared_ptr<ImageCodec>& codec);

    static std::vector<std::string> getRegisteredExtensions();

    static Image read(const std::string& fileName);
    
    static const ImageCodec& getBestCodec(const ImageView& image);

protected:
    virtual Image read(std::istream& stream, const std::string& fileName) const = 0;
    virtual void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const = 0;

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

// include/ImageApprovals/ImageWriter.hpp

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

// include/ImageApprovals.hpp

#ifdef ImageApprovals_IMPLEMENT

// src/ColorSpace.cpp

#include <ostream>

namespace ImageApprovals {

namespace {

struct LinearColorSpace : ColorSpace
{
    const char* getName() const override
    { return "Linear sRGB"; }
};

struct SRGBColorSpace : ColorSpace
{
    const char* getName() const override
    { return "sRGB"; }
};

}

const ColorSpace& ColorSpace::getLinearSRgb()
{
    static const LinearColorSpace instance;
    return instance;
}

const ColorSpace& ColorSpace::getSRgb()
{
    static const SRGBColorSpace instance;
    return instance;
}

std::ostream& operator <<(std::ostream& stream, const ColorSpace& colorSpace)
{
    return stream << colorSpace.getName();
}

}

// src/ColorSpaceUtils.hpp

#include <cstdint>
#include <cmath>

namespace ImageApprovals {

struct RgbPrimaries
{
    struct Primary
    {
        double x = 0.0, y = 0.0;

        Primary() = default;
        Primary(const Primary&) = default;

        Primary(double x, double y)
            : x(x), y(y)
        {}

        Primary& operator =(const Primary&) = default;

        bool approxEqual(const Primary& other) const;
    };

    Primary r, g, b;

    bool approxEqual(const RgbPrimaries& other) const;

    static RgbPrimaries getSRgbPrimaries();
};

bool isSRgbIccProfile(uint32_t profLen, const uint8_t* profData);

const ColorSpace* detectColorSpace(const RgbPrimaries& primaries, double gamma);

}

// src/CompareStrategy.cpp

#include <cstring>
#define NOMINMAX
#include <ApprovalTests.hpp>
#include <algorithm>

namespace ImageApprovals {

using ApprovalTests::StringUtils;

CompareStrategy::Result CompareStrategy::Result::makePassed()
{
    Result res;
    res.passed = true;
    return res;
}

CompareStrategy::Result CompareStrategy::Result::makeFailed(std::string leftInfo, std::string rightInfo)
{
    Result res;
    res.passed = false;
    res.leftImageInfo = std::move(leftInfo);
    res.rightImageInfo = std::move(rightInfo);
    return res;
}

CompareStrategy::Result CompareStrategy::compare(const ImageView& left, const ImageView& right) const
{
    Result result;

    if (!(result = compareInfos(left, right)).passed)
    {
        return result;
    }

    if (!(result = compareContents(left, right)).passed)
    {
        return result;
    }

    return Result::makePassed();
}

CompareStrategy::Result CompareStrategy::compareInfos(const ImageView& left, const ImageView& right) const
{
    Result result;
    result.passed = false;

    if (left.getPixelFormat() != right.getPixelFormat())
    {
        return Result::makeFailed(
            "pixel format = " + StringUtils::toString(left.getPixelFormat()),
            "pixel format = " + StringUtils::toString(right.getPixelFormat()));
    }

    if (left.getColorSpace() != right.getColorSpace())
    {
        return Result::makeFailed(
            "color space = " + StringUtils::toString(left.getColorSpace()),
            "color space = " + StringUtils::toString(right.getColorSpace()));
    }

    if (left.getSize() != right.getSize())
    {
        return Result::makeFailed(
            "size = " + StringUtils::toString(left.getSize()),
            "size = " + StringUtils::toString(right.getSize()));
    }

    return Result::makePassed();
}

ThresholdCompareStrategy::ThresholdCompareStrategy(AbsThreshold pixelFailThreshold, Percent maxFailedPixelsPercentage)
    : m_pixelFailThreshold(pixelFailThreshold), m_maxFailedPixelsPercentage(maxFailedPixelsPercentage)
{}

namespace {

float maxAbsDiff(const RGBA& left, const RGBA& right)
{
    float result = std::abs(left.r - right.r);
    result = std::max(result, std::abs(left.g - right.g));
    result = std::max(result, std::abs(left.b - right.b));
    result = std::max(result, std::abs(left.a - right.a));
    return result;
}

}

CompareStrategy::Result ThresholdCompareStrategy::compareContents(const ImageView& left, const ImageView& right) const
{
    const auto sz = left.getSize();

    uint32_t numAboveThreshold = 0;

    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto leftPixel = left.getPixel(x, y);
            const auto rightPixel = right.getPixel(x, y);

            const float diff = maxAbsDiff(leftPixel, rightPixel);
            if (diff > m_pixelFailThreshold.value)
            {
                ++numAboveThreshold;
            }
        }
    }

    const double numPixels = static_cast<double>(sz.width)* static_cast<double>(sz.height);
    const auto percentAboveThreshold = Percent((numAboveThreshold / numPixels) * 100.0);

    if (percentAboveThreshold > m_maxFailedPixelsPercentage)
    {
        std::string rightInfo
            = StringUtils::toString(numAboveThreshold) + " pixels (" + StringUtils::toString(percentAboveThreshold)
            + ") are above threshold = " + StringUtils::toString(m_pixelFailThreshold);

        return Result::makeFailed("reference image", rightInfo);
    }

    return Result::makePassed();
}

CompareStrategy::Result PixelPerfectCompareStrategy::compareContents(const ImageView& left, const ImageView& right) const
{
    const auto sz = left.getSize();
    const auto rowLen = left.getPixelFormat().getPixelStride() * sz.width;

    for(uint32_t y = 0; y < sz.height; ++y)
    {
        const auto leftRow = left.getRowPointer(y);
        const auto rightRow = right.getRowPointer(y);

        if(0 != std::memcmp(leftRow, rightRow, rowLen))
        {
            return Result::makeFailed("reference image", "different pixels in row " + std::to_string(y));
        }
    }

    return Result::makePassed();
}

}

// src/ExrImageCodec.hpp

namespace ImageApprovals {

class ExrImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override;

    int getScore(const std::string& extensionWithDot) const override;
    int getScore(const PixelFormat& pf, const ColorSpace& cs) const override;

protected:
    Image read(std::istream& stream, const std::string& fileName) const override;
    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override;
};

}

// src/Image.cpp

#include <cstring>
#include <fstream>

namespace ImageApprovals {

namespace {

size_t alignedSize(size_t baseSize, size_t alignment)
{
    return ((baseSize + alignment - 1) / alignment) * alignment;
}

size_t rowStride(const PixelFormat& fmt, const Size& sz, size_t rowAlignment)
{
    if (rowAlignment == 0)
    {
        return 0;
    }

    const auto rowSize = fmt.getPixelStride() * sz.width;
    return alignedSize(rowSize, rowAlignment);
}

}

Image::Image(Image&& other) noexcept
{
    *this = std::move(other);
}

Image::Image(const PixelFormat& format, const ColorSpace& colorSpace, const Size& size, size_t rowAlignment)
    : ImageView(format, colorSpace, size, rowStride(format, size, rowAlignment), nullptr), m_rowAlignment(rowAlignment)
{
    if (m_size.isZero())
    {
        throw ImageApprovalsError("Image size cannot be zero");
    }

    if (m_rowAlignment == 0)
    {
        throw ImageApprovalsError("Image row alignment must be greater than 0");
    }

    const size_t rowStride = getRowStride();

    m_data.reset(new uint8_t[rowStride * m_size.height]);
    m_dataPtr = m_data.get();

    for (uint32_t y = 0; y < m_size.height; ++y)
    {
        std::memset(getRowPointer(y), 0, rowStride);
    }
}

Image::~Image() noexcept = default;

Image& Image::operator =(Image&& rhs) noexcept
{
    if (this != &rhs)
    {
        m_format = rhs.m_format;
        rhs.m_format = nullptr;

        m_colorSpace = rhs.m_colorSpace;
        rhs.m_colorSpace = nullptr;

        m_size = rhs.m_size;
        rhs.m_size = {};

        m_rowStride = rhs.m_rowStride;
        rhs.m_rowStride = 0;

        m_rowAlignment = rhs.m_rowAlignment;
        rhs.m_rowAlignment = 0;

        m_data = std::move(rhs.m_data);

        m_dataPtr = m_data.get();
        rhs.m_dataPtr = nullptr;
    }

    return *this;
}

uint8_t* Image::getRowPointer(uint32_t y)
{
    if (y >= m_size.height)
    {
        throw ImageApprovalsError("Row index out of range");
    }

    const auto rowStride = getRowStride();
    return &m_data[rowStride * y];
}

void Image::flipVertically()
{
    const auto rowStride = getRowStride();

    std::unique_ptr<uint8_t[]> rowBuffer;
    rowBuffer.reset(new uint8_t[rowStride]);

    for (uint32_t y = 0; y < m_size.height / 2; ++y)
    {
        auto upperRow = getRowPointer(y);
        auto lowerRow = getRowPointer(m_size.height - y - 1);

        std::memcpy(rowBuffer.get(), upperRow, rowStride);
        std::memcpy(upperRow, lowerRow, rowStride);
        std::memcpy(lowerRow, rowBuffer.get(), rowStride);
    }
}

}

// src/ImageComparator.cpp

#include <sstream>
#include <iterator>
#include <stdexcept>

namespace ImageApprovals {

ImageComparator::Disposer::Disposer(std::vector<ApprovalTests::ComparatorDisposer> disposers)
    : m_disposers(std::move(disposers))
{}

ImageComparator::ImageComparator()
    : m_compareStrategy(std::make_shared<ThresholdCompareStrategy>())
{}

ImageComparator::ImageComparator(std::shared_ptr<CompareStrategy> comparator)
    : m_compareStrategy(std::move(comparator))
{}

namespace {

Image readImage(const std::string& which, const std::string& path)
{
    try
    {
        return ImageCodec::read(path);
    }
    catch (const std::exception & exc)
    {
        const auto msg =
            "Failed to read " + which + " image from \""
            + path + "\": " + exc.what();

        throw ApprovalTests::ApprovalException(msg);
    }
}

}

bool ImageComparator::contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const
{
    const Image receivedImg = readImage("received", receivedPath);
    const Image approvedImg = readImage("approved", approvedPath);

    const auto result = m_compareStrategy->compare(approvedImg, receivedImg);
    if (!result.passed)
    {
        throw ApprovalTests::ApprovalMismatchException(result.rightImageInfo, result.leftImageInfo);
    }

    return true;
}

ImageComparator::Disposer ImageComparator::registerForAllExtensions(std::shared_ptr<CompareStrategy> strategy)
{
    using namespace ApprovalTests;

    auto comparator = std::make_shared<ImageComparator>(std::move(strategy));

    const auto allExtensions = ImageCodec::getRegisteredExtensions();
    
    std::vector<ApprovalTests::ComparatorDisposer> disposers;
    disposers.reserve(allExtensions.size());

    transform(allExtensions.begin(), allExtensions.end(), std::back_inserter(disposers),
        [&](const std::string& ext) { return FileApprover::registerComparatorForExtension(ext, comparator); });

    return Disposer(std::move(disposers));
}

}

// src/ImageView.cpp

#include <stdexcept>
#include <ostream>

namespace ImageApprovals {

std::ostream& operator <<(std::ostream& stream, const Size& size)
{
    stream << "[" << size.width << ", " << size.height << "]";
    return stream;
}

ImageView::ImageView(const PixelFormat& format, const ColorSpace& colorSpace,
                     const Size& size, size_t rowStride, const uint8_t* data)
    : m_format(&format), m_colorSpace(&colorSpace), m_size(size),
      m_rowStride(rowStride), m_dataPtr(data)
{}

bool ImageView::isEmpty() const
{
    return m_format == nullptr;
}

const PixelFormat& ImageView::getPixelFormat() const
{
    if (!m_format)
    {
        throw ImageApprovalsError("Calling getPixelFormat on an empty ImageView");
    }

    return *m_format;
}

const ColorSpace& ImageView::getColorSpace() const
{
    if (!m_colorSpace)
    {
        throw ImageApprovalsError("Calling getColorSpace on an empty ImageView");
    }

    return *m_colorSpace;
}

const uint8_t* ImageView::getRowPointer(uint32_t index) const
{
    if (index >= m_size.height)
    {
        throw ImageApprovalsError("Row index out of range");
    }

    return m_dataPtr + m_rowStride * index;
}

RGBA ImageView::getPixel(uint32_t x, uint32_t y) const
{
    if (x >= m_size.width)
    {
        throw ImageApprovalsError("X out of range");
    }

    const auto rowPtr = getRowPointer(y);

    const auto& fmt = getPixelFormat();
    const auto pixelStride = fmt.getPixelStride();

    RGBA value;
    fmt.decode(rowPtr + pixelStride * x, rowPtr + m_rowStride, value);
    return value;
}

}

// src/PixelFormat.cpp

#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <ostream>
#include <array>

namespace ImageApprovals {

namespace {

float clamp(float x, float minX, float maxX)
{
    return std::min(maxX, std::max(minX, x));
}

template<typename T, size_t N>
std::array<float, N> normalize(const std::array<T, N>& src, float maxValue)
{
    std::array<float, N> dst;

    for (size_t i = 0; i < N; ++i)
    {
        dst[i] = clamp(src[i] / maxValue, 0.0f, 1.0f);
    }

    return dst;
}

template<size_t NumChannels, typename ChannelType>
struct GenericPixelFormat : PixelFormat
{
    size_t getNumberOfChannels() const override { return NumChannels; }
    size_t getPixelStride() const override { return NumChannels * sizeof(ChannelType); }

    bool isU8() const override { return std::is_same<ChannelType, uint8_t>::value; }
    bool isF32() const override { return std::is_same<ChannelType, float>::value; }

    std::array<ChannelType, NumChannels> decodeBytes(const uint8_t* src) const
    {
        std::array<ChannelType, NumChannels> dst;
        std::memcpy(dst.data(), src, NumChannels * sizeof(ChannelType));
        return dst;
    }
};

struct GrayU8PixelFormat : GenericPixelFormat<1, uint8_t>
{
    const char* getName() const override { return "GrayU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f)[0];
        outRgba = RGBA(v, v, v, 1);
    }
};

struct GrayAlphaU8PixelFormat : GenericPixelFormat<2, uint8_t>
{
    const char* getName() const override { return "GrayAlphaU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f);
        outRgba = RGBA(v[0], v[0], v[0], v[1]);
    }
};

struct RgbU8PixelFormat : GenericPixelFormat<3, uint8_t>
{
    const char* getName() const override { return "RgbU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f);
        outRgba = RGBA(v[0], v[1], v[2], 1);
    }
};

struct RgbAlphaU8PixelFormat : GenericPixelFormat<4, uint8_t>
{
    const char* getName() const override { return "RgbAlphaU8"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = normalize(decodeBytes(begin), 255.0f);
        outRgba = RGBA(v[0], v[1], v[2], v[3]);
    }
};

struct RgbF32PixelFormat : GenericPixelFormat<3, float>
{
    const char* getName() const override { return "RgbF32"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = decodeBytes(begin);
        outRgba = RGBA(v[0], v[1], v[2], 1);
    }
};

struct RgbAlphaF32PixelFormat : GenericPixelFormat<4, float>
{
    const char* getName() const override { return "RgbAlphaF32"; }

    void decode(const uint8_t* begin, RGBA& outRgba) const override
    {
        const auto v = decodeBytes(begin);
        outRgba = RGBA(v[0], v[1], v[2], v[3]);
    }
};

}

const uint8_t* PixelFormat::decode(const uint8_t* begin, const uint8_t* end, RGBA& outRgba) const
{
    const auto stride = getPixelStride();
    if (begin + stride > end)
    {
        throw ImageApprovalsError("begin + stride > end");
    }

    decode(begin, outRgba);
    return begin + stride;
}

const PixelFormat& PixelFormat::getGrayU8()
{
    static const GrayU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getGrayAlphaU8()
{
    static const GrayAlphaU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbU8()
{
    static const RgbU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbAlphaU8()
{
    static const RgbAlphaU8PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbF32()
{
    static const RgbF32PixelFormat instance;
    return instance;
}

const PixelFormat& PixelFormat::getRgbAlphaF32()
{
    static const RgbAlphaF32PixelFormat instance;
    return instance;
}

std::ostream& operator <<(std::ostream& stream, const PixelFormat& format)
{
    stream << format.getName();
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const RGBA& rgba)
{
    stream << "(" << rgba.r << ", " << rgba.g << ", " << rgba.b << ", " << rgba.a << ")";
    return stream;
}

}

// src/PngImageCodec.hpp

namespace ImageApprovals {

class PngImageCodec : public ImageCodec
{
public:
    std::string getFileExtensionWithDot() const override;

    int getScore(const std::string& extensionWithDot) const override;
    int getScore(const PixelFormat& pf, const ColorSpace& cs) const override;

protected:
    Image read(std::istream& stream, const std::string& fileName) const override;
    void write(const ImageView& image, std::ostream& stream, const std::string& fileName) const override;
};

}

// src/Units.cpp

#include <ostream>

namespace ImageApprovals {

std::ostream& operator <<(std::ostream& stream, const AbsThreshold& threshold)
{
    stream << threshold.value;
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const Percent& percent)
{
    stream << percent.value << "%";
    return stream;
}

}

// src/ColorSpaceUtils.cpp

#include <stdexcept>
#include <cstring>
#include <memory>
#include <array>

namespace ImageApprovals {

namespace {

bool approxEqual(double a, double b)
{
    return std::abs(a - b) <= 1e-5;
}

}

bool RgbPrimaries::Primary::approxEqual(const Primary& other) const
{
    return ImageApprovals::approxEqual(x, other.x)
        && ImageApprovals::approxEqual(y, other.y);
}

bool RgbPrimaries::approxEqual(const RgbPrimaries& other) const
{
    return r.approxEqual(other.r)
        && g.approxEqual(other.g)
        && b.approxEqual(other.b);
}

RgbPrimaries RgbPrimaries::getSRgbPrimaries()
{
    RgbPrimaries p;
    p.r = Primary(0.64, 0.33);
    p.g = Primary(0.30, 0.60);
    p.b = Primary(0.15, 0.06);
    return p;
}

namespace {

uint32_t fromBigEndian(uint32_t value)
{
    uint32_t result = 0;

    uint8_t beBytes[4];
    std::memcpy(beBytes, &value, 4);
    
    const uint8_t leBytes[4]{ beBytes[3], beBytes[2], beBytes[1], beBytes[0] };
    std::memcpy(&result, leBytes, 4);

    return result;
}

}

bool isSRgbIccProfile(uint32_t profLen, const uint8_t* profData)
{
    if (profLen < 132)
    {
        throw ImageApprovalsError("Incomplete ICC profile data");
    }

    const std::array<char, 4> rgbColorSpace{ 'R', 'G', 'B', ' ' };
    std::array<char, 4> colorSpace;
    std::memcpy(colorSpace.data(), profData + 16, 4);
    if (colorSpace != rgbColorSpace)
    {
        throw ImageApprovalsError("Color space in the ICC profile is not RGB");
    }

    uint32_t numTags = 0;
    std::memcpy(&numTags, profData + 128, 4);
    numTags = fromBigEndian(numTags);
    if ((128 + numTags * 12) > profLen)
    {
        throw ImageApprovalsError("Incomplete ICC profile data");
    }

    struct TagInfo
    {
        std::array<char, 4> signature;
        uint32_t offset;
        uint32_t size;
    };

    for (uint32_t tagIndex = 0; tagIndex < numTags; ++tagIndex)
    {
        TagInfo info{};
        std::memcpy(&info, profData + 132 + tagIndex * sizeof(TagInfo), sizeof(TagInfo));
        info.offset = fromBigEndian(info.offset);
        info.size = fromBigEndian(info.size);

        if ((info.offset + info.size) > profLen)
        {
            throw ImageApprovalsError("Incomplete ICC profile data");
        }

        // profileDescriptionTag 
        const std::array<char, 4> descSig{ 'd', 'e', 's', 'c' };
        if (info.signature == descSig)
        {
            const uint8_t* descPtr = profData + info.offset;

            uint32_t strLen = 0, strOffset = 0;

            std::memcpy(&strLen, descPtr + 20, 4);
            strLen = fromBigEndian(strLen);

            std::memcpy(&strOffset, descPtr + 24, 4);
            strOffset = fromBigEndian(strOffset);

            std::unique_ptr<char[]> str(new char[(strLen / 2) + 1]);
            str[strLen / 2] = '\0';

            for (uint32_t i = 0; i < strLen / 2; ++i)
            {
                str[i] = static_cast<char>((descPtr + strOffset)[1 + i * 2]);
            }

            if (std::strstr(str.get(), "sRGB") != nullptr)
            {
                return true;
            }
        }
    }

    return false;
}

const ColorSpace* detectColorSpace(const RgbPrimaries& primaries, double gamma)
{
    const auto sRgbPrimaries = RgbPrimaries::getSRgbPrimaries();

    if (!primaries.approxEqual(sRgbPrimaries))
    {
        return nullptr;
    }

    if (approxEqual(gamma, 1.0))
    {
        return &ColorSpace::getLinearSRgb();
    }
    else if (approxEqual(gamma, 1.0 / 2.2))
    {
        return &ColorSpace::getSRgb();
    }

    return nullptr;
}

}

// src/ExrImageCodec.cpp

#include <cstring>
#include <array>

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4996)
#endif

#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfIO.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/half.h>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace ImageApprovals {

namespace {

class InputStramAdapter : public Imf::IStream
{
public:
    InputStramAdapter(const std::string& fileName, std::istream& stream)
        : Imf::IStream(fileName.c_str()), m_stream(stream)
    {}

    bool isMemoryMapped() const override { return false; }

    bool read(char* c, int n) override
    {
        m_stream.read(c, n);

        const auto read_bytes = m_stream.gcount();
        if (read_bytes < n)
        {
            throw ImageApprovalsError("Not enough data");
        }

        return !m_stream.eof();
    }

    char* readMemoryMapped(int) override { throw ImageApprovalsError("Not memory mapped"); }

    Imf::Int64 tellg() override { return m_stream.tellg(); }

    void seekg(Imf::Int64 pos) override { m_stream.seekg(pos); }

    void clear() override { m_stream.clear(); }

private:
    std::istream& m_stream;
};

class OutputStreamAdapter : public Imf::OStream
{
public:
    OutputStreamAdapter(const std::string& fileName, std::ostream& stream)
        : Imf::OStream(fileName.c_str()), m_stream(stream)
    {}

    void write(const char* c, int n) override
    {
        m_stream.write(c, n);
    }

    Imf::Int64 tellp() override { return m_stream.tellp(); }

    void seekp(Imf::Int64 p) override { m_stream.seekp(p); }

private:
    std::ostream& m_stream;
};

}

namespace {

void copyPixels(size_t numChannels, const Size& sz, const Imf::Array2D<Imf::Rgba>& src, uint8_t* dst)
{
    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto srcPixel = src[x][y];

            const float value[]{
                static_cast<float>(srcPixel.r),
                static_cast<float>(srcPixel.g),
                static_cast<float>(srcPixel.b),
                static_cast<float>(srcPixel.a)
            };

            std::memcpy(dst, value, numChannels * 4);
            dst += numChannels * 4;
        }
    }
}

}

std::string ExrImageCodec::getFileExtensionWithDot() const
{
    return ".exr";
}

int ExrImageCodec::getScore(const std::string& extensionWithDot) const
{
    if(extensionWithDot == ".exr")
    {
        return 100;
    }
    
    return -1;
}

int ExrImageCodec::getScore(const PixelFormat& pf, const ColorSpace& cs) const
{
    if(!pf.isF32() || cs != ColorSpace::getLinearSRgb())
    {
        return -1;
    }

    return 100;
}

Image ExrImageCodec::read(std::istream& stream, const std::string& fileName) const
{
    InputStramAdapter streamAdapter(fileName, stream);

    Imf::RgbaInputFile file(streamAdapter);
    Imath::Box2i dw = file.dataWindow();

    const auto width = dw.max.x - dw.min.x + 1;
    const auto height = dw.max.y - dw.min.y + 1;

    Imf::Array2D<Imf::Rgba> pixels;
    pixels.resizeErase(width, height);

    file.setFrameBuffer(pixels[0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels(dw.min.y, dw.max.y);

    const PixelFormat* fmt = nullptr;

    switch (file.channels())
    {
    case Imf::WRITE_RGB:
        fmt = &PixelFormat::getRgbF32();
        break;
    case Imf::WRITE_RGBA:
        fmt = &PixelFormat::getRgbAlphaF32();
        break;
    default:
        throw ImageApprovalsError("Unsupported pixel format");
    }

    const Size imgSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    Image image(*fmt, ColorSpace::getLinearSRgb(), imgSize, 4);
    copyPixels(fmt->getNumberOfChannels(), imgSize, pixels, image.getPixelData());

    return image;
}

void ExrImageCodec::write(const ImageView& image, std::ostream& stream, const std::string& fileName) const
{
    const auto& fmt = image.getPixelFormat();

    if (!fmt.isF32())
    {
        throw ImageApprovalsError("EXR codec cannot write non-float pixels");
    }

    if (image.getColorSpace() != ColorSpace::getLinearSRgb())
    {
        throw ImageApprovalsError("EXR codec can write only images with linear color space");
    }

    Imf::RgbaChannels channels{};

    if (fmt == PixelFormat::getRgbF32())
    {
        channels = Imf::WRITE_RGB;
    }
    else if (fmt == PixelFormat::getRgbAlphaF32())
    {
        channels = Imf::WRITE_RGBA;
    }
    else
    {
        throw ImageApprovalsError("Unexpected pixel format");
    }

    OutputStreamAdapter streamAdapter(fileName, stream);

    const auto sz = image.getSize();
    int width = static_cast<int>(sz.width);
    int height = static_cast<int>(sz.height);

    const Imf::Header hdr(
        width, height,
        static_cast<float>(width) / height);

    Imf::RgbaOutputFile file(streamAdapter, hdr, channels);

    Imf::Array2D<Imf::Rgba> pixels;
    pixels.resizeErase(width, height);

    for (uint32_t y = 0; y < sz.height; ++y)
    {
        for (uint32_t x = 0; x < sz.width; ++x)
        {
            const auto srcPixel = image.getPixel(x, y);

            Imf::Rgba dstPixel;
            dstPixel.r = half(srcPixel.r);
            dstPixel.g = half(srcPixel.g);
            dstPixel.b = half(srcPixel.b);
            dstPixel.a = half(srcPixel.a);

            pixels[x][y] = dstPixel;
        }
    }

    file.setFrameBuffer(pixels[0], 1, width);
    file.writePixels(height);
}

}

// src/ImageCodec.cpp

#include <ApprovalTests.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>


namespace ImageApprovals {

namespace detail {

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

void ImageCodec::write(const std::string& fileName, const ImageView& image) const
{
    std::ofstream fileStream(fileName.c_str(), std::ios::binary);
    if (!fileStream)
    {
        throw ImageApprovalsError("Could not open file \"" + fileName + "\" for writing");
    }

    fileStream.exceptions(std::ios::badbit | std::ios::failbit);

    write(image, fileStream, fileName);
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

Image ImageCodec::read(const std::string& fileName)
{
    using ApprovalTests::FileUtils;
    const auto extWithDot = FileUtils::getExtensionWithDot(fileName);    

    const ImageCodec* codec = detail::findBestMatch(getImageCodecs(), extWithDot);

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

// src/PngImageCodec.cpp

#include <png.h>
#include <functional>
#include <cstring>
#include <istream>
#include <ostream>

namespace ImageApprovals {

namespace {

class OnExit
{
public:
    OnExit(std::function<void()> fn)
        : m_fn(std::move(fn))
    {}

    OnExit(const OnExit&) = delete;
    OnExit(OnExit&&) = delete;

    ~OnExit() noexcept
    {
        m_fn();
    }

private:
    std::function<void()> m_fn;
};

void PNGCBAPI readBytes(png_struct* png, png_byte* data, size_t len)
{
    auto& stream = *reinterpret_cast<std::istream*>(png_get_io_ptr(png));
    stream.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(len));
}

void PNGCBAPI writeBytes(png_struct* png, png_byte* data, size_t len)
{
    auto& stream = *reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
    stream.write(reinterpret_cast<const char*>(data), static_cast<std::streampos>(len));
}

void PNGCBAPI flush(png_struct* png)
{
    auto& stream = *reinterpret_cast<std::ostream*>(png_get_io_ptr(png));
    stream.flush();
}

const ColorSpace* detectColorSpace(png_struct* png, png_info* info)
{
    int intent = -1;
    if (png_get_sRGB(png, info, &intent) == PNG_INFO_sRGB)
    {
        return &ColorSpace::getSRgb();
    }

    char* iccpName = nullptr;
    int iccpCompressionType = PNG_COMPRESSION_TYPE_BASE;
    png_byte* iccpData = nullptr;
    uint32_t iccpDataLen = 0;
    const auto iCCP
        = png_get_iCCP(
            png, info,
            &iccpName,
            &iccpCompressionType,
            &iccpData, &iccpDataLen);

    if (iCCP == PNG_INFO_iCCP && isSRgbIccProfile(iccpDataLen, iccpData))
    {
        return &ColorSpace::getSRgb();
    }

    double white_x = 0.0, white_y = 0.0;
    RgbPrimaries prims;

    const auto cHRM
        = png_get_cHRM(
            png, info,
            &white_x, &white_y,
            &prims.r.x, &prims.r.y,
            &prims.g.x, &prims.g.y,
            &prims.b.x, &prims.b.y);

    double gamma = 0.0f;
    const auto gAMA = png_get_gAMA(png, info, &gamma);

    if (cHRM == PNG_INFO_cHRM && gAMA == PNG_INFO_gAMA)
    {
        return detectColorSpace(prims, gamma);
    }

    return &ColorSpace::getLinearSRgb();
}

void writePngComment(png_struct* png, png_info* info)
{
    std::string textKey = "Software";
    std::string textStr = "ImageApprovals";

    png_text text;
    text.compression = PNG_TEXT_COMPRESSION_NONE;
    text.key = &textKey[0];
    text.text = &textStr[0];
    text.text_length = textStr.size();
    text.itxt_length = 0;
    text.lang = nullptr;
    text.lang_key = nullptr;

    png_set_text(png, info, &text, 1);
}

}

std::string PngImageCodec::getFileExtensionWithDot() const
{
    return ".png";
}

int PngImageCodec::getScore(const std::string& extensionWithDot) const
{
    if(extensionWithDot == ".png")
    {
        return 100;
    }

    return -1;
}

int PngImageCodec::getScore(const PixelFormat& pf, const ColorSpace& cs) const
{
    if(!pf.isU8())
    {
        return -1;
    }

    if(cs != ColorSpace::getSRgb() && cs != ColorSpace::getLinearSRgb())
    {
        return -1;
    }

    return 100;
}

Image PngImageCodec::read(std::istream& stream, const std::string&) const
{
    Image image;
    png_struct* png = nullptr;
    png_info* info = nullptr;

    OnExit onExit([&]() {
        if (png)
        {
            png_destroy_read_struct(&png, info ? &info : nullptr, nullptr);
        }
    });

    if (!(png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)))
    {
        throw ImageApprovalsError("Failed to allocate PNG read struct");
    }
        
    if (!(info = png_create_info_struct(png)))
    {
        throw ImageApprovalsError("Failed to allocate PNG info struct");
    }

    if (setjmp(png_jmpbuf(png)))
    {
        throw ImageApprovalsError("Failed to read PNG image");
    }

    png_set_read_fn(png, &stream, &readBytes);

    png_read_png(png, info, 0, nullptr);

    const PixelFormat* format = nullptr;
    const Size imgSize{ png_get_image_width(png, info), png_get_image_height(png, info) };

    const int pngBitDepth = png_get_bit_depth(png, info);
    const int pngColorType = png_get_color_type(png, info);

    if (pngBitDepth != 8)
    {
        throw ImageApprovalsError("Unsupported PNG bit depth");
    }

    switch (pngColorType)
    {
    case PNG_COLOR_TYPE_GRAY:
        format = &PixelFormat::getGrayU8();
        break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
        format = &PixelFormat::getGrayAlphaU8();
        break;
    case PNG_COLOR_TYPE_RGB:
        format = &PixelFormat::getRgbU8();
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        format = &PixelFormat::getRgbAlphaU8();
        break;
    default:
        throw ImageApprovalsError("Unsupported PNG color type");
    }

    const ColorSpace* colorSpace = detectColorSpace(png, info);
    if (!colorSpace)
    {
        throw ImageApprovalsError("Unknown color space");
    }

    image = Image(*format, *colorSpace, imgSize);

    png_byte** rowPointers = png_get_rows(png, info);
    const size_t rowSize = format->getPixelStride() * imgSize.width;
    for (uint32_t y = 0; y < imgSize.height; ++y)
    {
        const png_byte* srcRow = rowPointers[y];
        uint8_t* dstRow = image.getRowPointer(y);
        std::memcpy(dstRow, srcRow, rowSize);
    }

    return image;
}

void PngImageCodec::write(const ImageView& image, std::ostream& stream, const std::string&) const
{
    const auto& fmt = image.getPixelFormat();

    if (!fmt.isU8())
    {
        throw ImageApprovalsError("Unable to write the image to PNG file");
    }

    png_struct* png = nullptr;
    png_info* info = nullptr;
        
    std::unique_ptr<png_const_bytep[]> rowPointers;

    OnExit onExit([&]() {
        if (png)
        {
            png_destroy_write_struct(&png, info ? &info : nullptr);
        }
    });

    if (!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)))
    {
        throw ImageApprovalsError("Failed to allocate PNG write struct");
    }

    if (!(info = png_create_info_struct(png)))
    {
        throw ImageApprovalsError("Failed to allocate PNG info struct");
    }

    if (setjmp(png_jmpbuf(png)))
    {
        throw ImageApprovalsError("Failed to write PNG image");
    }

    png_set_write_fn(png, &stream, &writeBytes, &flush);

    int pngColorType = 0;
    if (fmt == PixelFormat::getGrayU8())
    {
        pngColorType = PNG_COLOR_TYPE_GRAY;
    }
    else if (fmt == PixelFormat::getGrayAlphaU8())
    {
        pngColorType = PNG_COLOR_TYPE_GRAY_ALPHA;
    }
    else if (fmt == PixelFormat::getRgbU8())
    {
        pngColorType = PNG_COLOR_TYPE_RGB;
    }
    else if (fmt == PixelFormat::getRgbAlphaU8())
    {
        pngColorType = PNG_COLOR_TYPE_RGB_ALPHA;
    }
    else
    {
        throw ImageApprovalsError("Unexpected pixel format");
    }

    const auto sz = image.getSize();
    png_set_IHDR(
        png, info, sz.width, sz.height,
        8, pngColorType,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    if (image.getColorSpace() == ColorSpace::getSRgb())
    {
        png_set_sRGB_gAMA_and_cHRM(png, info, PNG_sRGB_INTENT_RELATIVE);
    }
    else if (image.getColorSpace() == ColorSpace::getLinearSRgb())
    {
        png_set_gAMA(png, info, 1.0);

        const auto p = RgbPrimaries::getSRgbPrimaries();
        png_set_cHRM(
            png, info,
            0.3127, 0.3291,
            p.r.x, p.r.y,
            p.g.x, p.g.y,
            p.b.x, p.b.y);
    }
    else
    {
        throw ImageApprovalsError("Unsupported ColorSpace");
    }

    writePngComment(png, info);

    rowPointers.reset(new png_const_bytep[sz.height]);
    for (uint32_t y = 0; y < sz.height; ++y)
    {
        rowPointers[y] = reinterpret_cast<const png_byte*>(image.getRowPointer(y));
    }

    png_set_rows(png, info, const_cast<png_bytepp>(rowPointers.get()));

    png_write_png(png, info, 0, nullptr);
}

}

#endif // ImageApprovals_IMPLEMENT

#endif // IMAGEAPPROVALS_HPP_INCLUDED
