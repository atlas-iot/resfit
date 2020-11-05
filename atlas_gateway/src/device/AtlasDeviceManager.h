#ifndef __ATLAS_DEVICE_MANAGER_H__
#define __ATLAS_DEVICE_MANAGER_H__

#include <unordered_map>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include "AtlasDevice.h"
#include "AtlasGateway.h"
#include "reputation_feedback/IAtlasFeedback.h"
#include "../cloud/AtlasDeviceCloud.h"
#include "../telemetry/AtlasTelemetry.h"

namespace atlas {

class AtlasDeviceManager
{

public:
    /**
    * @brief Get device manager instance
    * @return Device manager instance
    */
    static AtlasDeviceManager& getInstance();

    /**
    * @brief Install all policies for all devices
    * @return none
    */
    void installAllPolicies();

    /**
    * @brief Get device cloud back-end manager
    * @return Device cloud back-end manager
    */
    std::shared_ptr<AtlasDeviceCloud> getDeviceCloud() { return deviceCloud_; }

    /**
    * @brief Get the client device associated with the given identity. If the client
    * device does not exist in cache, but exists in db then it will be created. If not
    * cache return nullptr
    * @param[in] identity Client device identity
    * @return Instance to client device
    */
    AtlasDevice* getDevice(const std::string& identity);

    /**
     * @brief Get gateway device
     * @return Gateway device
     */
    AtlasGateway& getGateway() { return gateway_; }

    /**
    * @brief Parse each device
    * @param[in] cb For each callback
    * @return none
    */
    void forEachDevice(std::function<void(AtlasDevice&)> cb);

    /**
    * @brief Update data network reputation
    * @param[in] identity Device identity
    * @param[in] networkType Reputation data network type
    * @param[in] feedbackMatrix Feedback score matrix
    * @return none
    */
    void updateDataReputation(const std::string &identity,
                              AtlasDeviceNetworkType networkType,
                              std::vector<std::pair<AtlasDeviceFeatureType, double>> &feedbackMatrix);

   
    /**
    * @brief Update firewall statistics
    * @param[in] firewallStats New firewall statistics (delta from previous sample)
    * @return none
    */ 
    void updateFirewallStats(const AtlasFirewallStats &firewallStats);

    /**
    * @brief Get the most trusted device within a categiry (sensor type)
    * @param[in] networkType Reputation network type
    * @return Identity of the most trusted device
    */
    std::string getTrustedDevice(AtlasDeviceNetworkType networkType);

    AtlasDeviceManager(const AtlasDeviceManager&) = delete;
    AtlasDeviceManager& operator=(const AtlasDeviceManager&) = delete;

private:
    /**
    * @brief Ctor for device manager
    * @return none
    */
    AtlasDeviceManager();
    
    /**
    * @brief Dtor for device manager
    * @return none
    */
    ~AtlasDeviceManager();

    /**
    * @brief Firewall-statistics alarm callback
    * @return none
    */
    void firewallStatisticsAlarmCallback();

    /**
    * @brief System reputation alarm callback
    * @return none
    */
    void sysRepAlarmCallback();

    /**
    * @brief Sub function for reputation alarm callback
    * @return none
    */
    void subAlarmCallback(AtlasDevice& device);

   /**
   * @brief Init system reputation
   * @param[in] device Client device
   * @return none
   */
    void initSystemReputation(AtlasDevice &device);

   /**
   * @brief Init system statistics
   * @param[in] device Client device
   * @return none
   */
    void initSystemStatistics(AtlasDevice &device);
   
   /**
   * @brief Init data reputation
   * @param[in] device Client device
   * @return none
   */
    void initDataReputation(AtlasDevice &device);

   /**
   * @brief Init device commands from database
   * @param[in] device Client device
   * @return none
   */
    void initDeviceCommands(AtlasDevice &device);

    /**
    * @brief Update reputation order to deliver the most trusted
    *        device within a category
    * @param[in] networkType Reputation network type (sensor type)
    * @return none
    */
    void updateReputationOrder(AtlasDeviceNetworkType networkType);

    /* Client devices */
    std::unordered_map<std::string, AtlasDevice> devices_;

    /* Stateful reputation feedback for system behaviour */
    std::unordered_map<std::string, std::vector<std::unique_ptr<IAtlasFeedback>>> feedback_;

    /* Telemetry manager which handles the telemetry CoAP resources and pushes the data into the devices */
    AtlasTelemetry telemetry_;

    /* Cloud back-end manager */
    std::shared_ptr<AtlasDeviceCloud> deviceCloud_;

    /* Firewall-statistics alarm */
    AtlasAlarm fsAlarm_;

    /* System reputation alarm */
    AtlasAlarm sysRepAlarm_;

    /* Holds the most trusted device within a sensor category */
    std::unordered_map<AtlasDeviceNetworkType, std::string> trustedDevices_;

    /* Gateway device */
    AtlasGateway gateway_;
};

} // namespace atlas

#endif /* __ATLAS_DEVICE_MANAGER_H__ */
