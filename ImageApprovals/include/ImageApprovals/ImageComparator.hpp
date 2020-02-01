#ifndef IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED

#include "Units.hpp"
#include <string>

namespace ImageApprovals {

class ImageView;

class ImageComparator
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

    virtual ~ImageComparator() = default;

    Result compare(const ImageView& left, const ImageView& right) const;

protected:
    virtual Result compareInfos(const ImageView& left, const ImageView& right) const;
    virtual Result compareContents(const ImageView& left, const ImageView& right) const = 0;
};

class ThresholdImageComparator : public ImageComparator
{
public:
    ThresholdImageComparator() = default;
    ThresholdImageComparator(AbsThreshold pixelFailThreshold, Percent maxFailedPixelsPercentage);

    AbsThreshold getPixelFailThreshold() const { return m_pixelFailThreshold; }
    Percent getMaxFailedPixelsPercentage() const { return m_maxFailedPixelsPercentage; }

protected:
    Result compareContents(const ImageView& left, const ImageView& right) const override;

private:
    AbsThreshold m_pixelFailThreshold = AbsThreshold(0.004);
    Percent m_maxFailedPixelsPercentage = Percent(0.1);
};

}

#endif // IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED
