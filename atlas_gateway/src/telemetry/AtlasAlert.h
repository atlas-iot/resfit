#ifndef __ATLAS_ALERT_H__
#define __ATLAS_ALERT_H__

#include <string>
#include "../coap/AtlasCoapResponse.h"

namespace atlas {

const uint16_t ATLAS_ALERT_COAP_TIMEOUT_MS = 5000;

class AtlasAlert
{
public:
    /**
    * @brief Ctor for telemetry alert
    * @param[in] deviceIdentity Client device identity
    * @param[in] path Telemetry alert CoAP path
    * @return none
    */
    AtlasAlert(const std::string &deviceIdentity, const std::string &path);
    
    /**
    * @brief Push telemetry alert to client device
    * @return none
    */
    void push();

    /**
    * @brief Dtor for alert
    * @return none
    */
    virtual ~AtlasAlert();
protected:
     /**
    * @brief CoAP response callbacl
    * @param[in] respStatus CoAP response status
    * @param[in] resp_payload CoAP response payload
    * @param[in] resp_payload_len CoAP response payload length
    * @return none
    */
    void respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len);
   
    virtual void pushCommand(const std::string &path) = 0;

    /* CoAP context*/
    void *coapToken_;
private:

    /* Client device identity */
    std::string deviceIdentity_;

    /* Telemetry alert CoAP path */
    std::string path_;
};

} // namespace atlas

#endif /* __ATLAS_ALERT_H__ */
