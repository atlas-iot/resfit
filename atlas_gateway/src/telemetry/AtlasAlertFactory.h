#ifndef __ATLAS_ALERT_FACTORY_H__
#define __ATLAS_ALERT_FACTORY_H__

#include "AtlasPushAlert.h"
#include "AtlasThresholdAlert.h"

namespace atlas {

class AtlasAlertFactory
{
public:
    /**
    * @brief Create telemetry push alert
    * @param[in] type Telemetry feature type
    * @param[in] deviceIdentity Client device identity
    * @return Telemetry push alert
    */
    static AtlasPushAlert* getPushAlert(const std::string& type, const std::string &deviceIdentity_);

    /**
    * @brief Create telemetry threshold alert
    * @param[in] type Telemetry feature type
    * @param[in] deviceIdentity Client device identity
    * @return Telemetry threshold alert
    */
    static AtlasThresholdAlert* getThresholdAlert(const std::string& type, const std::string &deviceIdentity_);
};

} // namespace atlas

#endif /* __ATLAS_ALERT_FACTORY_H__ */
