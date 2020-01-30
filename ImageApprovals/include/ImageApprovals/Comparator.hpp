#ifndef IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED

#include "ImageComparator.hpp"
#include <ApprovalTests.hpp>

namespace ImageApprovals {

class Comparator : public ApprovalTests::ApprovalComparator
{
public:
    Comparator();
    explicit Comparator(std::unique_ptr<ImageComparator> comparator);

    bool contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const override;

private:
    std::unique_ptr<ImageComparator> m_comparator;
};

}

#endif // IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED