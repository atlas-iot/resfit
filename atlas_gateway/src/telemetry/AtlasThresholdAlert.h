#ifndef __ATLAS_THRESHOLD_ALERT_H__
#define __ATLAS_THRESHOLD_ALERT_H__

#include <string>
#include "AtlasAlert.h"
#include "../coap/AtlasCoapResponse.h"

namespace atlas {

class AtlasThresholdAlert: public AtlasAlert
{
public:
    /**
    * @brief Ctor for telemetry alert
    * @param[in] deviceIdentity Client device identity
    * @param[in] path Telemetry alert CoAP path
    * @param[in] scanRate Scan rate value
    * @param[in] threshold Threshold value
    * @return none
    */
    AtlasThresholdAlert(const std::string &deviceIdentity, const std::string &path,
                        uint16_t scanRate, const std::string &threshold);

private:
    void pushCommand(const std::string &url);

    /* Telemetry alert internal scan rate */
    uint16_t scanRate_;

    /* Telemetry alert threshold (string because it is application specific) */
    std::string threshold_;
};

} // namespace atlas

#endif /* __ATLAS_THRESHOLD_ALERT_H__ */
