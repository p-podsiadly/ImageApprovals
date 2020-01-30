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

template<typename ConcreteImageComparator, typename... Arguments>
std::shared_ptr<Comparator> makeImageComparator(Arguments&&... args)
{
    std::unique_ptr<ImageComparator> imgComparator;
    imgComparator.reset(new ConcreteImageComparator(std::forward<Arguments>(args)...));

    return std::make_shared<Comparator>(std::move(imgComparator));
}

}

#endif // IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED