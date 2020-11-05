#ifndef __ATLAS_FEATURE_REPUTATION_H__
#define __ATLAS_FEATURE_REPUTATION_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"

namespace atlas {

class AtlasFeatureReputation
{

public:
    /**
    * @brief Ctor for feature reputation
    * @return none
    */
    AtlasFeatureReputation();

    /**
    * @brief Register feature reputation resource
    * @return none
    */
    void start();

    /**
    * @brief Un-register feature reputation resource
    * @return none
    */
    void stop();

private:

    /**
    * @brief Feature reputation client callback
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
    AtlasCoapResponse requestReputationCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                                AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                                uint8_t **respPayload, size_t *respPayloadLen);

    /**
    * @brief Feedback callback
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
    AtlasCoapResponse feedbackReputationCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                                 AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                                 uint8_t **respPayload, size_t *respPayloadLen);

    /* FEATURE command CoAP resource*/
    AtlasCoapResource featureReputationResource_;

    /* Feedback command CoAP resource*/
    AtlasCoapResource receiveFeedbackResource_;
};

} // namespace atlas

#endif /* __ATLAS_FEATURE_REPUTATION_H__ */
