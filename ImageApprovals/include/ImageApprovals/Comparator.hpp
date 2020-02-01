#ifndef IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED

#include "ImageComparator.hpp"
#include <ApprovalTests.hpp>

namespace ImageApprovals {

class Comparator : public ApprovalTests::ApprovalComparator
{
public:
    class Disposer;

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

    template<typename ConcreteImageComparator, typename... Arguments>
    static Disposer registerForAllExtensions(Arguments&&... args)
    {
        std::unique_ptr<ImageComparator> imgComparator(
            new ConcreteImageComparator(std::forward<Arguments>(args)...));
        return registerForAllExtensionsImpl(std::move(imgComparator));
    }

private:
    std::unique_ptr<ImageComparator> m_comparator;

    static Disposer registerForAllExtensionsImpl(std::unique_ptr<ImageComparator> imageComparator);
};

class Comparator::Disposer
{
public:
    explicit Disposer(std::vector<ApprovalTests::ComparatorDisposer> disposers);
    Disposer(const Disposer&) = delete;
    Disposer(Disposer&&) = default;

    Disposer& operator =(const Disposer&) = delete;
    Disposer& operator =(Disposer&&) = delete;

private:
    std::vector<ApprovalTests::ComparatorDisposer> m_disposers;
};

}

#endif // IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED