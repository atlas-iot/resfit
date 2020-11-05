#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "AtlasDeviceManager.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"
#include "../reputation_impl/AtlasReputation_NaiveBayes.h"
#include "../register/AtlasRegister.h"
#include "reputation_feedback/AtlasRegistrationFeedback.h"
#include "reputation_feedback/AtlasKeepaliveFeedback.h"
#include "reputation_feedback/AtlasPacketsFeedback.h"
#include "../sql/AtlasSQLite.h"
#include "../reputation_impl/AtlasDeviceFeatureManager.h"

namespace atlas {

namespace {

const int ATLAS_FIREWALL_STATISTICS_INTERVAL_MS = 60000;
const int ATLAS_SYSTEM_REPUTATION_INTERVAL_MS = 60000;

/* Reputation feature weights (importance) for system network */
const double ATLAS_VALID_PACKETS_WEIGHT = 0.3;
const double ATLAS_REGISTER_TIME_WEIGHT = 0.4;
const double ATLAS_KEEPALIVE_PACKETS_WEIGHT = 0.3;

/* Reputation feature weights (importance) for data network */
const double ATLAS_SENSOR_WEIGHT = 0.7;
const double ATLAS_RESP_TIME_WEIGHT = 0.3;

/* System reputation threshold value */
const double ATLAS_SYSTEM_REPUTATION_THRESHOLD = 0.8;
/* Data reputation threshold value */
const double ATLAS_DATA_REPUTATION_THRESHOLD = 0.8;

/* Global reputation score weight:
 * data network score * <weight1> + system network score * <weight2>, where
 weight1 + weight2 = 1  */
const double ATLAS_SYSTEM_REPUTATION_WEIGHT = 0.7;
const double ATLAS_DATA_REPUTATION_WEIGHT = 0.3;

} // anonymous namespace

AtlasDeviceManager& AtlasDeviceManager::getInstance()
{
    static AtlasDeviceManager instance;

    return instance;
}

AtlasDeviceManager::AtlasDeviceManager(): deviceCloud_(new AtlasDeviceCloud()),
                                          fsAlarm_("AtlasDeviceManagerFirewall", ATLAS_FIREWALL_STATISTICS_INTERVAL_MS,
                                                   false, boost::bind(&AtlasDeviceManager::firewallStatisticsAlarmCallback, this)),
                                          sysRepAlarm_("AtlasDeviceManagerSysRep", ATLAS_SYSTEM_REPUTATION_INTERVAL_MS,
                                                       false, boost::bind(&AtlasDeviceManager::sysRepAlarmCallback, this)),
                                          gateway_(deviceCloud_)
{
    /* Start firewall statistics alarm */
    fsAlarm_.start();
    /* Start system reputation alarm */
    sysRepAlarm_.start();
    /* Add reputation type for the most trusted devices */
    trustedDevices_[AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE] = "";
}

void AtlasDeviceManager::updateFirewallStats(const AtlasFirewallStats &firewallStats)
{
    ATLAS_LOGGER_DEBUG("Update firewall statistics for device with identity " +
                       firewallStats.getClientId());

    AtlasDevice* device = AtlasDeviceManager::getInstance().getDevice(firewallStats.getClientId());
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " +
                           firewallStats.getClientId());
        return;
    }

    device->getFirewallStats().addRuleDroppedPkts(firewallStats.getRuleDroppedPkts());
    device->getFirewallStats().addRulePassedPkts(firewallStats.getRulePassedPkts());
    device->getFirewallStats().addTxDroppedPkts(firewallStats.getTxDroppedPkts());
    device->getFirewallStats().addTxPassedPkts(firewallStats.getTxPassedPkts());

    /* Save to firewall statistics into the database */
    bool result = AtlasSQLite::getInstance().updateStats(device->getIdentity(),
                                                         device->getFirewallStats());
    if(!result)
        ATLAS_LOGGER_ERROR("Uncommited update on statistics data");

    ATLAS_LOGGER_DEBUG("Sync firewall statistics with the cloud back-end");
    device->syncFirewallStatistics();
}

void AtlasDeviceManager::firewallStatisticsAlarmCallback()
{
    ATLAS_LOGGER_INFO("Firewall-statistics alarm callback");

    forEachDevice([] (AtlasDevice& device) 
                     {
                         if(device.getPolicy()) {
                             AtlasPubSubAgent::getInstance().getFirewallRuleStats(device.getPolicy()->getClientId());
                         }
                     });
}

std::string AtlasDeviceManager::getTrustedDevice(AtlasDeviceNetworkType networkType)
{
    /* If reputation network type does not exist (a new entry cannot be added) */
    if (trustedDevices_.find(networkType) == trustedDevices_.end())
        return "";

    return trustedDevices_[networkType];
}

void AtlasDeviceManager::updateReputationOrder(AtlasDeviceNetworkType networkType)
{
    double repVal;
    double repValMax = 0;
    std::string mostTrusted;

    ATLAS_LOGGER_INFO("Update global reputation order");

    forEachDevice([&] (AtlasDevice& device)
                      {
                          if (!device.hasReputation(networkType))
                              return;
                        
                          AtlasDeviceFeatureManager& sysRep = device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM); 
                          AtlasDeviceFeatureManager& dataRep = device.getReputation(networkType); 
                          repVal = sysRep.getReputationScore() * ATLAS_SYSTEM_REPUTATION_WEIGHT +
                                   dataRep.getReputationScore() * ATLAS_DATA_REPUTATION_WEIGHT;
                          if (repVal > repValMax) {
                              repValMax = repVal;
                              mostTrusted = device.getIdentity();
                          }
                      });

    trustedDevices_[networkType] = mostTrusted;
}

void AtlasDeviceManager::subAlarmCallback(AtlasDevice& device)
{
    std::vector<std::pair<AtlasDeviceFeatureType, double>> feedbackMatrix;
    /* Parse each system reputation feedback element and get feedback for device */
    for (auto &feedbackElem : feedback_[device.getIdentity()])
        feedbackMatrix.push_back(std::pair<AtlasDeviceFeatureType, double>(feedbackElem->getType(),
                                                                           feedbackElem->getFeedback()));

    /* Compute system reputation using naive-bayes */
    double repVal = AtlasReputationNaiveBayes::computeReputation(device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM),
                                                                 feedbackMatrix);
    ATLAS_LOGGER_INFO("System reputation for device with identity " + device.getIdentity() + " is " + std::to_string(repVal));
    device.syncReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM);

    /* Update into db*/
    bool result = AtlasSQLite::getInstance().updateBayesParams(device.getIdentity(), 
                                                               (int) AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM, 
                                                               device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM));
    if(!result)
        ATLAS_LOGGER_ERROR("Uncommited update on naiveBayes params for System");
}

void AtlasDeviceManager::sysRepAlarmCallback()
{
    ATLAS_LOGGER_INFO("System reputation alarm callback");

    forEachDevice([this] (AtlasDevice& device) 
                         {     
                            subAlarmCallback(device);
                         });

    /* After the system reputation is changed, update the reputation order */
    for (auto it = trustedDevices_.begin(); it != trustedDevices_.end(); ++it)
        updateReputationOrder(it->first);
}

void AtlasDeviceManager::updateDataReputation(const std::string &identity,
                                              AtlasDeviceNetworkType networkType,
                                              std::vector<std::pair<AtlasDeviceFeatureType, double>> &feedbackMatrix)
{
    if (devices_.find(identity) == devices_.end()) {
        ATLAS_LOGGER_ERROR("Cannot update data reputation network for un-registered devices");
        return;
    }

    if (networkType <= AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_START ||
        networkType >= AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_MAX) {
        ATLAS_LOGGER_ERROR("Invalid data reputation network type");
        return;
    }

    if (feedbackMatrix.empty()) {
        ATLAS_LOGGER_INFO("Data reputation network cannot be updated because of empty feedback matrix");
        return;
    }

    AtlasDevice &device = devices_[identity];
    if (!device.hasReputation(networkType)) {
        ATLAS_LOGGER_ERROR("Skip data reputation update: device with identity " + identity + 
                           " does not have the required network type");
        return;
    }
    
    AtlasDeviceFeatureManager &dataReputation = device.getReputation(networkType);

    /* Compute system reputation using naive-bayes */
    double repVal = AtlasReputationNaiveBayes::computeReputation(dataReputation, feedbackMatrix);
    
    ATLAS_LOGGER_INFO("Data reputation for device with identity " + device.getIdentity() +
                      " is " + std::to_string(repVal));

    device.syncReputation(networkType);

    /* Update into db*/
    bool result = AtlasSQLite::getInstance().updateBayesParams(device.getIdentity(),
                                                               (int) networkType,
                                                               dataReputation);
    if(!result)
        ATLAS_LOGGER_ERROR("Uncommited update on naiveBayes params for System");

    /* Update sensor reputation order */
    updateReputationOrder(networkType);
}


void AtlasDeviceManager::initSystemReputation(AtlasDevice &device)
{
    bool result;

    /* Add default features for the system reputation */
    AtlasDeviceFeatureManager &systemReputation = device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM);
    
    systemReputation.updateFeedbackThreshold(ATLAS_SYSTEM_REPUTATION_THRESHOLD);

    result = AtlasSQLite::getInstance().checkDeviceForFeatures(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM);
    if (result) {
        /* Get from db*/
        ATLAS_LOGGER_INFO("Get data from local.db");
        result = AtlasSQLite::getInstance().selectBayesParams(device.getIdentity(), 
                                                             (int)AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM,
                                                             systemReputation);
        if (result) {
            /* Compute system reputation using naive-bayes */
            double repVal = AtlasReputationNaiveBayes::computeReputation(systemReputation);
            ATLAS_LOGGER_INFO("System reputation for device with identity " + device.getIdentity() +
                              " is " + std::to_string(repVal));
        } else
            ATLAS_LOGGER_ERROR("Uncommited select on naiveBayes params data");
    } else {
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME,
                                    ATLAS_REGISTER_TIME_WEIGHT);
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS,
                                    ATLAS_KEEPALIVE_PACKETS_WEIGHT);
        systemReputation.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS,
                                    ATLAS_VALID_PACKETS_WEIGHT);

        /* Insert into db*/
        ATLAS_LOGGER_INFO("Insert data into local.db");
        result = AtlasSQLite::getInstance().insertBayesParams(device.getIdentity(),
                                                              (int) AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM,
                                                              systemReputation);
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited insert for naiveBayes params data");
    }

    /* Add feedack for system reputation */
    std::unique_ptr<IAtlasFeedback> regFeedback(new AtlasRegistrationFeedback(device,
                                                                              ATLAS_SYSTEM_REPUTATION_INTERVAL_MS / 1000));
    feedback_[device.getIdentity()].push_back(std::move(regFeedback));

    std::unique_ptr<IAtlasFeedback> kaFeedback(new AtlasKeepaliveFeedback(device,
                                                                          ATLAS_SYSTEM_REPUTATION_INTERVAL_MS / ATLAS_KEEP_ALIVE_INTERVAL_MS));
    feedback_[device.getIdentity()].push_back(std::move(kaFeedback));

    std::unique_ptr<IAtlasFeedback> pktFeedback(new AtlasPacketsFeedback(device));
    feedback_[device.getIdentity()].push_back(std::move(pktFeedback));
}

void AtlasDeviceManager::initSystemStatistics(AtlasDevice &device)
{
    /*Get stats from DB if exists*/
    bool result = AtlasSQLite::getInstance().checkDeviceForStats(device.getIdentity());
    if(result) {
        /* select from db */   
        result = AtlasSQLite::getInstance().selectStats(device.getIdentity(), device.getFirewallStats());
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited select on statistics data");
    } else {
        /* insert into db */
        result = AtlasSQLite::getInstance().insertStats(device.getIdentity(), device.getFirewallStats());
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited insert on statistics data");
    }
}

void AtlasDeviceManager::initDataReputation(AtlasDevice &device)
{
    /* Add default features for the data reputation */
    AtlasDeviceFeatureManager &dataReputation = device.getReputation(AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE);
    
    dataReputation.updateFeedbackThreshold(ATLAS_DATA_REPUTATION_THRESHOLD);
     
    bool result = AtlasSQLite::getInstance().checkDeviceForFeatures(device.getIdentity(), (int)AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE);
    if (result) {
        /* Get from db*/
        ATLAS_LOGGER_INFO("Get data from local.db");
        result = AtlasSQLite::getInstance().selectBayesParams(device.getIdentity(), 
                                                              (int)AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE,
                                                              dataReputation);
        if (result) {
            /* Compute system reputation using naive-bayes */
            double repVal = AtlasReputationNaiveBayes::computeReputation(dataReputation);
            ATLAS_LOGGER_INFO("Data reputation for device with identity " + device.getIdentity() +
                              " is " + std::to_string(repVal));
        } else
            ATLAS_LOGGER_ERROR("Uncommited select on naiveBayes params data");
    } else {

        dataReputation.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_SENSOR, ATLAS_SENSOR_WEIGHT);
        dataReputation.addFeature(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_RESP_TIME, ATLAS_RESP_TIME_WEIGHT);

        /* Insert into db*/
        ATLAS_LOGGER_INFO("Insert data into local.db");
        result = AtlasSQLite::getInstance().insertBayesParams(device.getIdentity(),
                                                              (int) AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE,
                                                              dataReputation);
        if(!result)
            ATLAS_LOGGER_ERROR("Uncommited insert for naiveBayes params data");

    }
}

void AtlasDeviceManager::initDeviceCommands(AtlasDevice &device)
{
    /* Load device commands from database that have not been executed on client */
    bool result = AtlasSQLite::getInstance().checkDeviceCommandByIdentity(device.getIdentity());
    if (result) {
        /* Get from db*/
        ATLAS_LOGGER_INFO("Get unexecuted device commands from local.db");
        
        result = AtlasSQLite::getInstance().selectDeviceCommand(device);
        if (result) {

            ATLAS_LOGGER_INFO(std::to_string(device.sizeRecvCommand()) + " device commands for device with identity " + device.getIdentity() + " are in memory");
        } else {

            ATLAS_LOGGER_ERROR("Uncommited select on device commands in selectDeviceCommand function");
        }
    } else {
        ATLAS_LOGGER_INFO("Device with identity " + device.getIdentity() + " has no device commands in local database");
    }
}

AtlasDevice* AtlasDeviceManager::getDevice(const std::string& identity)
{
    if (devices_.find(identity) == devices_.end()) {
        
        std::string psk = AtlasSQLite::getInstance().selectDevicePsk(identity);
        if(psk == "") {
            ATLAS_LOGGER_ERROR("PSK is empty for identity " + identity);
            return nullptr; 
        }

        ATLAS_LOGGER_INFO1("New client device created with identity ", identity);

        devices_[identity] = AtlasDevice(identity, 
                                         psk,
                                         deviceCloud_);

        initSystemReputation(devices_[identity]);
        initDataReputation(devices_[identity]);
        initSystemStatistics(devices_[identity]);
        initDeviceCommands(devices_[identity]);
    }

    return &devices_[identity];
}

void AtlasDeviceManager::forEachDevice(std::function<void(AtlasDevice&)> cb)
{
    for (auto it = devices_.begin(); it != devices_.end(); ++it)
        cb(it->second); 
}

void AtlasDeviceManager::installAllPolicies()
{
    forEachDevice([] (AtlasDevice& device)
                     { 
                         if(device.getPolicy())
                             AtlasPubSubAgent::getInstance().installFirewallRule(device.getIdentity(),
                                                                                 device.getPolicy());
                     });
}

AtlasDeviceManager::~AtlasDeviceManager()
{
    /* Stop firewall statistics alarm */
    fsAlarm_.cancel();
    /* Stop system reputation alarm */
    sysRepAlarm_.cancel();
}

} // namespace atlas
