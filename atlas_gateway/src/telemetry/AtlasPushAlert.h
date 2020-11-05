#ifndef __ATLAS_PUSH_ALERT_H__
#define __ATLAS_PUSH_ALERT_H__

#include <string>
#include "AtlasAlert.h"
#include "../coap/AtlasCoapResponse.h"


namespace atlas {

class AtlasPushAlert: public AtlasAlert
{
public:
    /**
    * @brief Ctor for telemetry alert
    * @param[in] deviceIdentity Client device identity
    * @param[in] path Telemetry alert CoAP path
    * @param[in] pushRate Push rate value
    * @return none
    */
    AtlasPushAlert(const std::string &deviceIdentity, const std::string &path,
                   uint16_t pushRate);

private:
    void pushCommand(const std::string &url);

    /* Telemetry alert external push to gateway rate */
    uint16_t pushRate_;
};

} // namespace atlas

#endif /* __ATLAS_PUSH_ALERT_H__ */
