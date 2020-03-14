#ifndef IMAGEAPPROVALS_QT5INTEGRATION_HPP_INCLUDED
#define IMAGEAPPROVALS_QT5INTEGRATION_HPP_INCLUDED

#ifdef ImageApprovals_CONFIG_WITH_QT5

#include "ImageView.hpp"

class QImage;

namespace ImageApprovals {

ImageView makeView(const QImage& image);

}

#endif // ImageApprovals_CONFIG_WITH_QT5

#endif // IMAGEAPPROVALS_QT5INTEGRATION_HPP_INCLUDED
