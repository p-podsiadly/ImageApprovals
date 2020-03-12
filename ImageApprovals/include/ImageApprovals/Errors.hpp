#ifndef IMAGEAPPROVALS_ERRORS_HPP_INCLUDED
#define IMAGEAPPROVALS_ERRORS_HPP_INCLUDED

#include <exception>
#include <string>

namespace ImageApprovals {

class ImageApprovalsError : public std::exception
{
public:
    explicit ImageApprovalsError(std::string message)
        : m_message(std::move(message))
    {}

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

private:
    std::string m_message;
};

}

#endif // IMAGEAPPROVALS_ERRORS_HPP_INCLUDED
