#ifndef IMAGEAPPROVALS_COMPARESTRATEGY_HPP_INCLUDED
#define IMAGEAPPROVALS_COMPARESTRATEGY_HPP_INCLUDED

#include "Units.hpp"
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

class BitwiseCompareStrategy : public CompareStrategy
{
public:
    BitwiseCompareStrategy() = default;

protected:
    Result compareContents(const ImageView& left, const ImageView& right) const override;
};

}

#endif // IMAGEAPPROVALS_COMPARESTRATEGY_HPP_INCLUDED
