#ifndef IMAGEAPPROVALS_COLORSPACE_HPP_INCLUDED
#define IMAGEAPPROVALS_COLORSPACE_HPP_INCLUDED

#include <iosfwd>

namespace ImageApprovals {

class ColorSpace
{
public:
    virtual ~ColorSpace() = default;

    bool operator ==(const ColorSpace& rhs) const
    { return this == &rhs; }

    bool operator !=(const ColorSpace& rhs) const
    { return !(*this == rhs); }

    virtual const char* getName() const = 0;

    static const ColorSpace& getLinear();

    static const ColorSpace& getSRGB();
};

std::ostream& operator <<(std::ostream& stream, const ColorSpace& colorSpace);

}

#endif // IMAGEAPPROVALS_COLORSPACE_HPP_INCLUDED
