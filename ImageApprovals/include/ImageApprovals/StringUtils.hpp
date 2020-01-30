#ifndef IMAGEAPPROVALS_STRINGUTILS_HPP_INCLUDED
#define IMAGEAPPROVALS_STRINGUTILS_HPP_INCLUDED

#include <sstream>

namespace ImageApprovals {

template<typename T>
std::string toString(const T& v)
{
    std::ostringstream stream;
    stream << v;
    return stream.str();
}

}

#endif // IMAGEAPPROVALS_STRINGUTILS_HPP_INCLUDED
