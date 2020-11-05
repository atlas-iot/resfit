#ifndef __ATLAS_TELEMETRY_H__
#define __ATLAS_TELEMETRY_H__

#include "../coap/AtlasCoapResponse.h"
#include "../coap/AtlasCoapMethod.h"
#include "../coap/AtlasCoapResource.h"
#include "../commands/AtlasCommandBatch.h"

namespace atlas {

class AtlasTelemetry {

public:
    /**
    * @brief Ctor for telemetry
    * @return none
    */
    AtlasTelemetry();

private:
    /**
    * @brief Telemetry feature client callback
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
    AtlasCoapResponse featureCallback(const std::string &path, const std::string &identity, const std::string &psk,
                                       AtlasCoapMethod method, const uint8_t* reqPayload, size_t reqPayloadLen,
                                       uint8_t **respPayload, size_t *respPayloadLen);

    /**
    * @brief Get telemetry feature information from command
    * @param[in] path Telemetry feature URI path
    * @param[in] cmd CoAP command
    * @return A pair consisting in a feature key and a feature value
    */
    std::pair<std::string,std::string> getFeature(const std::string &path, AtlasCommand &cmd);

    /* Hostname telemetry CoAP resource*/
    AtlasCoapResource hostnameResource_;

    /* Kernel info telemetry CoAP resource*/
    AtlasCoapResource kernInfoResource_;
    
    /* Sysinfo uptime telemetry CoAP resource*/
    AtlasCoapResource uptimeResource_;
    
    /* Sysinfo totalram telemetry CoAP resource*/
    AtlasCoapResource totalramResource_;
    
    /* Sysinfo freeram telemetry CoAP resource*/
    AtlasCoapResource freeramResource_;
    
    /* Sysinfo sharedram telemetry CoAP resource*/
    AtlasCoapResource sharedramResource_;
    
    /* Sysinfo bufferram telemetry CoAP resource*/
    AtlasCoapResource bufferramResource_;
    
    /* Sysinfo totalswap telemetry CoAP resource*/
    AtlasCoapResource totalswapResource_;
    
    /* Sysinfo freeswap telemetry CoAP resource*/
    AtlasCoapResource freeswapResource_;
    
    /* Sysinfo procs telemetry CoAP resource*/
    AtlasCoapResource procsResource_;
        
    /* Sysinfo load1 telemetry CoAP resource*/
    AtlasCoapResource load1Resource_;
    
    /* Sysinfo load5 telemetry CoAP resource*/
    AtlasCoapResource load5Resource_;

    /* Sysinfo load15 telemetry CoAP resource*/
    AtlasCoapResource load15Resource_;
    
    /* Packets info packets_per_minute telemetry CoAP resource*/
    AtlasCoapResource packetsPerMinuteResource_;
    
    /* Packets info packets_avg telemetry CoAP resource*/
    AtlasCoapResource packetsAvgResource_;
};

} // namespace atlas

#endif /* __ATLAS_TELEMETRY_H__ */
