#ifndef __ATLAS_COAP_REQUEST_H__
#define __ATLAS_COAP_REQUEST_H__

#include <coap2/coap.h>
#include <functional>
#include "AtlasCoapResponse.h"

namespace atlas {

typedef std::function<void(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)> coap_request_callback_t;

class AtlasCoapRequest
{

public:

    /**
     * @brief Ctor for CoAP request
     * @param[in] context CoAP context
     * @param[in] session CoAP session
     * @param[in] callback Request callback
     * @return none
     */
    AtlasCoapRequest(coap_context_t *context, coap_session_t *session, uint32_t token,
                     coap_request_callback_t callback);
    
    /**
     * @brief Ctor for CoAP request
     * @return none
     */ 
    AtlasCoapRequest();

    /**
     * @brief Getter for CoAP context
     * @return CoAP context
     */
    coap_context_t *getContext() const { return context_; }

    /**
     * @brief Getter for CoAP session
     * @return CoAP session
     */
    coap_session_t *getSession() const {return session_; }

    /**
     * @brief Getter for request callback
     * @return request callback
     */
    coap_request_callback_t getCallback() const { return callback_; }

    /**
    * @brief Get CoAP token
    * @return CoAP token
    */
    uint32_t getToken() const { return token_; }

private:
    /* CoAP context */
    coap_context_t *context_;
    
    /* CoAP session */
    coap_session_t *session_;
    
    /* CoAP request callback */
    coap_request_callback_t callback_;

    /* CoAP request token */
    uint32_t token_;
};

} // namespace atlas

#endif /* __ATLAS_COAP_REQUEST_H__ */

