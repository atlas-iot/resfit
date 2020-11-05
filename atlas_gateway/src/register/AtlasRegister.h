#ifndef __ATLAS_REGISTER_H__
#define __ATLAS_REGISTER_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../identity/AtlasIdentity.h"
#include "../device/AtlasDeviceManager.h"

namespace atlas {

/* Client device keep-alive register interval */
const int ATLAS_KEEP_ALIVE_INTERVAL_MS = 20000;

class AtlasRegister
{

public:
    AtlasRegister();

    void start();
    void stop();

private:
    /**
    * @brief Register client callback
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
    AtlasCoapResponse registerCallback(const std::string &path, const std::string &pskIdentity, const std::string &psk,
                                       AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                       uint8_t **respPayload, size_t *respPayloadLen);
    
    /**
    * @brief Keepalive client callback
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
    AtlasCoapResponse keepaliveCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                        AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                        uint8_t **respPayload, size_t *respPayloadLen);

    /**
    * @brief Keep-alive alarm callback
    * @return none
    */
    void keepaliveAlarmCallback();

    /* REGISTER command CoAP resource*/
    AtlasCoapResource registerResource_;

    /* KEEPALIVE command CoAP resource*/
    AtlasCoapResource keepAliveResource_;

    /* Keep-alive alarm */
    AtlasAlarm kaAlarm_;
};

} // namespace atlas

#endif /* __ATLAS_REGISTER_H__ */
