#ifndef __ATLAS_DEVICE_CLOUD_H__
#define __ATLAS_DEVICE_CLOUD_H__

#include <string>
#include "../alarm/AtlasAlarm.h"

namespace atlas {

class AtlasDeviceCloud
{
public:
    /**
    * @brief Ctor for Atlas device cloud
    * @return none
    */
    AtlasDeviceCloud();

    /**
    * @brief Update a device information to the cloud back-end
    * @param[in] identity Device identity
    * @param[in] jsonInfo Information to be updated (JSON serialized)
    * @return none
    */
    void updateDevice(const std::string &identity, const std::string &jsonInfo);

    /**
    * @brief Update information for all device to the cloud back-end
    * @return none
    */
    void allDevicesUpdate();

    /**
     * @brief Update gateway information to the cloud back-end
     * @param[in] jsonInfo Information to be updated (JSON serialized)
     * @return none
     */
    void updateGateway(const std::string &jsonInfo);

    /**
     * @brief Execute a full gateway update to the cloud back-end
     * @return none
     */
    void gatewayFullUpdate();

private:
    /**
    * @brief Sync alarm callback
    * @return none
    */
    void syncAlarmCallback();

    /* Cloud sync alarm */
    AtlasAlarm syncAlarm_;
    
    /* Indicates if a cloud sync is scheduled */
    bool syncScheduled_;
};

} // namespace atlas

#endif /*__ATLAS_DEVICE_CLOUD_H__ */
