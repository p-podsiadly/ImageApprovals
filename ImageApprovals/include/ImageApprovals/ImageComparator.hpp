#ifndef IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED

#include "Units.hpp"
#include <string>

namespace ImageApprovals {

class ImageView;

struct CmpMessage
{
    std::string left;
    std::string right;
};

class ImageComparator
{
public:
    virtual ~ImageComparator() = default;

    bool compare(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const;

protected:
    virtual bool compareInfos(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const;
    virtual bool compareContents(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const = 0;
};

class ThresholdImageComparator : public ImageComparator
{
public:
    ThresholdImageComparator(
        AbsThreshold pixelFailThreshold = AbsThreshold(0.004),
        Percent maxFailedPixelsPercentage = Percent(0.1));

    AbsThreshold getPixelFailThreshold() const { return m_pixelFailThreshold; }
    Percent getMaxFailedPixelsPercentage() const { return m_maxFailedPixelsPercentage; }

protected:
    bool compareContents(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const override;

private:
    AbsThreshold m_pixelFailThreshold;
    Percent m_maxFailedPixelsPercentage;
};

}

#endif // IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED
