#ifndef IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED
#define IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED

#include "CompareStrategy.hpp"
#include <ApprovalTests.hpp>

namespace ImageApprovals {

class Comparator : public ApprovalTests::ApprovalComparator
{
public:
    class Disposer
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

    Comparator();
    explicit Comparator(std::shared_ptr<CompareStrategy> comparator);

    bool contentsAreEquivalent(std::string receivedPath, std::string approvedPath) const override;

    template<typename ConcreteImageComparator, typename... Arguments>
    static std::shared_ptr<Comparator> make(Arguments&&... args)
    {
        std::shared_ptr<CompareStrategy> imgComparator(
            new ConcreteImageComparator(std::forward<Arguments>(args)...));

        return std::make_shared<Comparator>(std::move(imgComparator));
    }

    template<typename Strategy, typename... Arguments>
    static Disposer registerForAllExtensions(Arguments&&... args)
    {
        auto strategy = std::make_shared<Strategy>(std::forward<Arguments>(args)...);
        return registerForAllExtensions(strategy);
    }

    static Disposer registerForAllExtensions(std::shared_ptr<CompareStrategy> strategy);

private:
    std::shared_ptr<CompareStrategy> m_compareStrategy;
};

}

#endif // IMAGEAPPROVALS_COMPARATOR_HPP_INCLUDED