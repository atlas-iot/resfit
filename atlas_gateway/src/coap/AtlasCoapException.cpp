#include "AtlasCoapException.h"

AtlasCoapException::AtlasCoapException(const std::string &message) : message_(message) {}

const char *AtlasCoapException::what() const throw()
{
    return message_.c_str();
}
