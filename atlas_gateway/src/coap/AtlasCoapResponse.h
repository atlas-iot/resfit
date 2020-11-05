#ifndef __ATLAS_COAP_RESPONSE_H__
#define __ATLAS_COAP_RESPONSE_H__

namespace atlas {

enum AtlasCoapResponse
{
    ATLAS_COAP_RESP_OK = 200,
    ATLAS_COAP_RESP_NOT_FOUND = 404,
    ATLAS_COAP_RESP_NOT_ACCEPTABLE = 406,
    ATLAS_COAP_RESP_TIMEOUT,
    ATLAS_COAP_RESP_RESET,
    ATLAS_COAP_RESP_UNKNOWN,
};

} // namespace atlas

#endif /* __ATLAS_COAP_RESPONSE_H__ */
