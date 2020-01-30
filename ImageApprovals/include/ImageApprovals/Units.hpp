#ifndef IMAGEAPPROVALS_UNITS_HPP_INCLUDED
#define IMAGEAPPROVALS_UNITS_HPP_INCLUDED

#include <iosfwd>

namespace ImageApprovals {

namespace detail {

template<typename ValueType, typename Tag>
struct Unit
{
    ValueType value = ValueType(0);

    constexpr Unit() = default;
    constexpr Unit(const Unit&) = default;

    constexpr explicit Unit(ValueType value)
        : value(value)
    {}

    Unit& operator =(const Unit&) = default;

    bool operator ==(const Unit& rhs) const{ return value == rhs.value; }
    bool operator !=(const Unit& rhs) const{ return value != rhs.value; }

    bool operator <(const Unit& rhs) const{ return value < rhs.value; }
    bool operator >(const Unit& rhs) const{ return value > rhs.value; }

    bool operator <=(const Unit& rhs) const{ return value <= rhs.value; }
    bool operator >=(const Unit& rhs) const{ return value >= rhs.value; }
};

}

using AbsThreshold = detail::Unit<double, struct AbsThresholdTag>;
using Percent = detail::Unit<double, struct PercentTag>;

std::ostream& operator <<(std::ostream& stream, const AbsThreshold& threshold);
std::ostream& operator <<(std::ostream& stream, const Percent& percent);

}

#endif // IMAGEAPPROVALS_UNITS_HPP_INCLUDED
