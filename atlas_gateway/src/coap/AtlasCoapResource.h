#ifndef __ATLAS_COAP_RESOURCE_H__
#define __ATLAS_COAP_RESOURCE_H__

#include <stdint.h>
#include <functional>
#include "AtlasCoapMethod.h"
#include "AtlasCoapResponse.h"

namespace atlas {

typedef std::function<AtlasCoapResponse (const std::string&, const std::string&, const std::string&, AtlasCoapMethod, const uint8_t*, size_t, uint8_t **, size_t *)> resource_callback_t;

class AtlasCoapResource {

public:
    /**
     * @brief Ctor for CoAP server resource
     * @param[in] uri CoAP URI
     * @param[in] method CoAP method
     * @param[in] callback Resource callback
     * @return none
     */
    AtlasCoapResource(const std::string &uri, AtlasCoapMethod method, resource_callback_t callback); 

    AtlasCoapResource();

    /**
     * @brief Getter for CoAP URI
     * @return CoAP URI
     */
    std::string getUri() const { return this->uri_; }

    /**
     * @brief Getter for CoAP method
     * @return CoAP method
     */
    AtlasCoapMethod getCoapMethod() const { return this->method_; }

    /**
     * @brief Getter for resource callback
     * @return Resource callback
     */
    resource_callback_t getCallback() const { return this->callback_; }

private:
    /* CoAP URI */
    std::string uri_;
    /* CoAP method */
    AtlasCoapMethod method_;
    /* Resource callback */
    resource_callback_t callback_;
};

} // namespace atlas

#endif /* __ATLAS_COAP_RESOURCE_H__ */
