#ifndef IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED

#include <ApprovalTests.hpp>

namespace ImageApprovals {

class ImageComparator
{
public:
    virtual ~ImageComparator() = default;
};

class Comparator : public ApprovalTests::ApprovalComparator
{
public:
    bool contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const override;
};

}

#endif // IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED