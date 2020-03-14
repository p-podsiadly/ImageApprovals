#ifndef IMAGEAPPROVALS_QIMAGEVIEW_HPP_INCLUDED
#define IMAGEAPPROVALS_QIMAGEVIEW_HPP_INCLUDED

#ifdef ImageApprovals_CONFIG_WITH_QT5

#include "ImageView.hpp"
#include "Errors.hpp"
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

#endif // ImageApprovals_CONFIG_WITH_QT5

#endif // IMAGEAPPROVALS_QIMAGEVIEW_HPP_INCLUDED
