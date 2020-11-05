#ifndef __ATLAS_POLICY_H__
#define __ATLAS_POLICY_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"

namespace atlas {

class AtlasPolicy
{

public:
    AtlasPolicy();

    void start();
    void stop();

private:
    /**
    * @brief Firewall policy client callback
    * @param[in] path CoAP URI path
    * @param[in] pskIdentity PSK identity (extracted from the DTLS transport layer)
    * @param[in] psk Pre-shared key
    * @param[in] method CoAP method
    * @param[in] reqPayload Request payload
    * @param[in] reqPayloadLen Request payload length
    * @param[out] respPayload Response payload
    * @param[out] respPayloadLen Response payload length
    * @return CoAP response status
    */
    AtlasCoapResponse firewallPolicyCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                        AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                        uint8_t **respPayload, size_t *respPayloadLen);

    /* FIREWALL command CoAP resource*/
    AtlasCoapResource firewallPolicyResource_;
};

} // namespace atlas

#endif /* __ATLAS_POLICY_H__ */
