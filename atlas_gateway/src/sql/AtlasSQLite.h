#ifndef __ATLAS_SQLITE_H__
#define __ATLAS_SQLITE_H__

#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include <queue>
#include "../logger/AtlasLogger.h"
#include "../reputation_impl/AtlasDeviceFeatureManager.h"
#include "../statistics/AtlasFirewallStats.h"
#include "../commands/AtlasCommandDevice.h"
#include "../claim_approve/AtlasApprove.h"
#include "../device/AtlasDevice.h"

namespace atlas {

const std::string ATLAS_DB_PATH = "local.db";

class AtlasSQLite
{

public:
    /**
    * @brief Get SQLite instance
    * @return SQLite instance
    */
    static AtlasSQLite& getInstance();

    /**
    * @brief Open connection for an existing/new created database
    * @param[in] databasePath Database path
    * @return connection state
    */
    bool openConnection(const std::string &databasePath); 

    /**
    * @brief Close connection with database
    * @return none
    */
    void closeConnection();

    /**
    * @brief Getter bConnected
    * @return bConnected
    */
    bool isConnected();

    /**
    * @brief Initialize data base
    * @param[in] databasePath Database path
    * @return true on success
    */
    bool initDB(const std::string &databasePath);

    /**
    * @brief Execute query on database: insert device
    * @param[in] identity - device identity
    * @param[in] psk - device psk
    * @return true on success, false on error
    */
    bool insertDevice(const std::string &identity, const std::string &psk);

    /**
    * @brief Execute query on database: select, get psk based on device identity
    * @param[in] identity Device identity
    * @return psk
    */
    std::string selectDevicePsk(const std::string &identity);

    /**
    * @brief Execute query on database: insert network & features params
    * @param[in] identity - device identity
    * @param[in] networkTypeId - naive bayes network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool insertBayesParams(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: insert statistics
    * @param[in] identity - device identity
    * @param[in] stats AtlasFirewallStats
    * @return true on success, false on error
    */
    bool insertStats(const std::string &identity, const AtlasFirewallStats &stats);

    /**
    * @brief Execute query on database: select, get network & features params based on device identity and network type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManager
    * @return true on success, false on error
    */
    bool selectBayesParams(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: select, get statistics based on device identity
    * @param[in] identity Device identity
    * @param[in] stats AtlasFirewallStats
    * @return true on success, false on error
    */
    bool selectStats(const std::string &identity, AtlasFirewallStats &stats);

    /**
    * @brief Execute query on database: update network & features params based on device identity and network type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool updateBayesParams(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: update statistics based on device identity
    * @param[in] identity Device identity
    * @param[in] stats AtlasFirewallStats
    * @return true on success, false on error
    */
    bool updateStats(const std::string &identity, const AtlasFirewallStats &stats);

    /**
    * @brief Check if a device has related features in db
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @return true if features exist, false on error or not exist
    */
    bool checkDeviceForFeatures(const std::string &identity, int networkTypeId);

    /**
    * @brief Check if a device has related statistics in db
    * @param[in] identity Device identity
    * @return true if features exist, false on error or not exist
    */
    bool checkDeviceForStats(const std::string &identity);

    /**
     * @brief Execute query on database: insert owner using secretKey and identity
     * @param[in] secretKey Owner secret key
     * @param[in] identity Owner identity
     * @return true on success, false on error
     */
    bool insertOwner(const std::string &secretKey, const std::string &identity);

    /**
     * Execute query on database: select owner (gateway claim) information
     * @param[out] secretKey Owner secret key
     * @param[out] identity Owner identity
     * @return true on success, false on error
     */
    bool selectOwnerInfo(std::string &secretKey, std::string &identity);

    /**
     * @brief Execute query on database: insert command device
     * @param[in] sequenceNumber Command sequence number
     * @param[in] commandType Command Type
     * @param[in] commandPayload Command payload
     * @param[in] deviceIdentity Device identity
     * @return true on success, false on error
     */
    bool insertDeviceCommand(const uint32_t sequenceNumber, const std::string &commandType,
                             const std::string &commandPayload, const std::string &deviceIdentity);
    /**
     * @brief Execute query on database: check if a command device exists
     * @param[in] sequenceNumber Command sequence number
     * @return true on success, false on error
     */
    bool checkDeviceCommandBySeqNo(const uint32_t sequenceNumber);

    /**
    * @brief Check if a device has related device commands in db
    * @param[in] identity Device identity
    * @return true if features exist, false on error or not exist
    */
    bool checkDeviceCommandByIdentity(const std::string &identity);

    /**
    * @brief Check if a device command is executed
    * @param[in] sequenceNumber Command sequence number
    * @return true if it is executed, false on error or not executed
    */
    bool checkDeviceCommandForExecution(const uint32_t sequenceNumber);

    /**
     * @brief Execute query on database: set field 'IsExecuted' to 1
     * @param[in] sequenceNumber Command sequence number
     * @return true on success, false on error
     */
    bool markExecutedDeviceCommand(const uint32_t sequenceNumber);

    /**
    * @brief Execute query on database: select, get device commands based on device identity
    * @param[in] device Device client
    * @return true on success, false on error
    */
    bool selectDeviceCommand(AtlasDevice &device);

    /**
     * @brief Execute query on database: delete a device command by sequence number
     * @param[in] sequenceNumber Command sequence number
     * @return true on success, false on error
     */
    bool deleteDeviceCommand(const uint32_t sequenceNumber);

    /**
     * @brief Execute query on database: get max sequence number from deviceCommand table
     * @param[in] sequenceNumber Command sequence number
     * @return true on success, false on error
     */
    bool getMaxSequenceNumber();

    AtlasSQLite(const AtlasSQLite&) = delete;
    AtlasSQLite& operator=(const AtlasSQLite&) = delete;

private:

    /**
    * @brief Ctor
    * @return none
    */
    AtlasSQLite();

    /**
    * @brief Dtor. It disconnect opened database
    * @return none
    */
    ~AtlasSQLite();

    /**
    * @brief Execute query on database: insert network
    * @param[in] identity - device identity
    * @param[in] networkTypeId - naive bayes network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool insertNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: insert feature
    * @param[in] identity - device identity
    * @param[in] networkTypeId - naive bayes network type
    * @param[in] featureTypeId - feature type(features for control/dataPlane)
    * @param[in] successTrans - number of successful transactions
    * @return true on success, false on error
    */
    bool insertFeatures(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);
    
    /**
    * @brief Execute query on database: select, get network params based on device identity
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool selectNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: select, get features params based on device identity and network type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManager
    * @return true on success, false on error
    */
    bool selectFeatures(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: update network params based on device identity and network type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManage
    * @return true on success, false on error
    */
    bool updateNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
    * @brief Execute query on database: update features params based on device identity, network type and feature type
    * @param[in] identity Device identity
    * @param[in] networkTypeId Network type
    * @param[in] manager AtlasDeviceFeatureManager
    * @return true on success, false on error
    */
    bool updateFeatures(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager);

    /**
     * @brief Execute query on database: update global sequence number
     * @param[in] sequenceNumber Command sequence number
     * @return true on success, false on error
     */
    bool updateMaxSequenceNumber(const uint32_t sequenceNumber);

    /*status of the connection*/
    bool isConnected_;	 

    /*SQLite connection object*/
    sqlite3 *pCon_;		
    
};
} // namespace atlas

#endif /* __ATLAS_SQLITE_H__ */
