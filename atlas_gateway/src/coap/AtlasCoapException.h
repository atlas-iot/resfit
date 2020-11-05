#ifndef __ATLAS_COAP_EXCEPTION_H__
#define __ATLAS_COAP_EXCEPTION_H__

#include <exception>
#include <string>

class AtlasCoapException: public std::exception
{

public:
    AtlasCoapException(const std::string &message);

    virtual const char* what() const throw();

private:
    std::string message_;
};

#endif /* __ATLAS_COAP_EXCEPTION_H__ */
