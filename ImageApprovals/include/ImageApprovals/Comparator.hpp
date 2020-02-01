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

    template<typename ConcreteImageComparator, typename... Arguments>
    static std::shared_ptr<Comparator> make(Arguments&&... args)
    {
        std::unique_ptr<ImageComparator> imgComparator(
            new ConcreteImageComparator(std::forward<Arguments>(args)...));

        return std::make_shared<Comparator>(std::move(imgComparator));
    }

private:
    std::unique_ptr<ImageComparator> m_comparator;
};

}

#endif // IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED