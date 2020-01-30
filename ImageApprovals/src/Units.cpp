#include <ImageApprovals/Units.hpp>
#include <ostream>

namespace ImageApprovals {

std::ostream& operator <<(std::ostream& stream, const AbsThreshold& threshold)
{
    stream << threshold.value;
    return stream;
}

std::ostream& operator <<(std::ostream& stream, const Percent& percent)
{
    stream << percent.value << "%";
    return stream;
}

}