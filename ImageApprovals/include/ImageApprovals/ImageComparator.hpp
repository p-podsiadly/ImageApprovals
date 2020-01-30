#ifndef IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED

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
    ThresholdImageComparator(float pixelFailThreshold = 0.004f, float maxFailedPixelsPercentage = 0.1f);

    float getPixelFailThreshold() const { return m_pixelFailThreshold; }
    float getMaxFailedPixelsPercentage() const { return m_maxFailedPixelsPercentage; }

protected:
    bool compareContents(const ImageView& left, const ImageView& right, CmpMessage& outMessage) const override;

private:
    float m_pixelFailThreshold;
    float m_maxFailedPixelsPercentage;
};

}

#endif // IMAGEAPPROVALS_IMAGECOMPARATOR_HPP_INCLUDED
