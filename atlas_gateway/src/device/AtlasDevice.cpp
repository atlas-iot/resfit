#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include "AtlasDevice.h"
#include "../logger/AtlasLogger.h"
#include "../telemetry/AtlasTelemetryInfo.h"
#include "../telemetry/AtlasAlertFactory.h"
#include "../pubsub_agent/AtlasPubSubAgent.h"
#include "../reputation_impl/AtlasDeviceFeatureType.h"
#include "../sql/AtlasSQLite.h"
#include "../commands/AtlasCommandBatch.h"
#include "../coap/AtlasCoapClient.h"

namespace atlas {

namespace {

/* Keep-alive counter initial value */
const int ATLAS_KEEP_ALIVE_COUNTER = 4;
/* JSON register event key */
const std::string ATLAS_REGISTER_JSON_KEY = "registered";
/* JSON last register time key */
const std::string ATLAS_LAST_REGISTER_TIME_JSON_KEY = "lastRegisterTime";
/* JSON last keep-alive time key */
const std::string ATLAS_LAST_KEEPALIVE_TIME_JSON_KEY = "lastKeepAliveTime";
/* JSON IP and port key */
const std::string ATLAS_IP_PORT_JSON_KEY = "ipPort";
/* JSON firewall stat-droppedPkts key */
const std::string ATLAS_FIREWALLSTAT_DROPPEDPKTS_JSON_KEY = "droppedPkts";
/* JSON firewall stat-passedPkts key */
const std::string ATLAS_FIREWALLSTAT_PASSEDPKTS_JSON_KEY = "passedPkts";
/* JSON system reputation key */
const std::string ATLAS_SYSTEM_REPUTATION_JSON_KEY = "systemReputation";
/* JSON data reputation key */
const std::string ATLAS_DATA_REPUTATION_JSON_KEY = "temperatureReputation";
/* device commmand default path on client */
const std::string ATLAS_PUSH_APPROVED_COMMAND = "client/approved/command/push";
/* Number of accepted timeouts before stop sending device command */
const uint8_t ATLAS_PUSH_COMMAND_RETRY_NO = 3;

const uint16_t ATLAS_APPROVED_COMMAND_COAP_TIMEOUT_MS = 5000;

} // anonymous namespace

AtlasDevice::AtlasDevice(const std::string &identity,
                         const std::string &psk,
                         std::shared_ptr<AtlasDeviceCloud> deviceCloud) : identity_(identity),
                                                                          psk_(psk),
                                                                          deviceCloud_(deviceCloud),
                                                                          registered_(false),
                                                                          regIntervalSec_(0),
                                                                          keepAlivePkts_(0),
                                                                          coapDeviceCmdToken_(nullptr),
                                                                          deviceCmdTimeouts_(0)
{
    /* Install default alerts */
    installDefaultAlerts();
    /* Init publish-subscribe client id */
    stats_.setClientId(identity);
}

AtlasDevice::AtlasDevice() : identity_(""), registered_(false), regIntervalSec_(0), keepAlivePkts_(0),
                             coapDeviceCmdToken_(nullptr), deviceCmdTimeouts_(0) {}

void AtlasDevice::uninstallPolicy()
{
    if (policy_) {
        /* Remove firewall rule from publish-subscribe broker */
        AtlasPubSubAgent::getInstance().removeFirewallRule(policy_->getClientId());
        /* Explicit delete policy */
        policy_.reset();
    }
}

void AtlasDevice::installDefaultAlerts()
{
    AtlasPushAlert *pushAlert;
    AtlasThresholdAlert *thresholdAlert;

    ATLAS_LOGGER_INFO1("Install default telemetry alerts for client with identity ", identity_);
    
    /* Install default push alerts */
    
    /* Sysinfo number of processes */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo uptime */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_UPTIME, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_UPTIME] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo freeram */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_FREERAM, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_FREERAM] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo shared RAM */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_SHAREDRAM, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_SHAREDRAM] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo buffer RAM */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_BUFFERRAM, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_BUFFERRAM] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo total swap */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_TOTALSWAP, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_TOTALSWAP] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo free swap */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_FREESWAP, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_FREESWAP] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo load1 */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_LOAD1, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_LOAD1] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo load5 */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_LOAD5, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_LOAD5] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Sysinfo load15 */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_SYSINFO_LOAD15, identity_);
    pushAlerts_[TELEMETRY_SYSINFO_LOAD15] = std::unique_ptr<AtlasAlert>(pushAlert);

    /* Data plane packets per minute*/
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE, identity_);
    pushAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE] = std::unique_ptr<AtlasAlert>(pushAlert);
    /* Data plane average packets length */
    pushAlert = AtlasAlertFactory::getPushAlert(TELEMETRY_PACKETS_INFO_PACKETS_AVG, identity_);
    pushAlerts_[TELEMETRY_PACKETS_INFO_PACKETS_AVG] = std::unique_ptr<AtlasAlert>(pushAlert);

    /* Install default threshold alerts */

    /* Sysinfo number of processes */
    thresholdAlert = AtlasAlertFactory::getThresholdAlert(TELEMETRY_SYSINFO_PROCS, identity_);
    thresholdAlerts_[TELEMETRY_SYSINFO_PROCS] = std::unique_ptr<AtlasAlert>(thresholdAlert);
}

void AtlasDevice::pushAlerts()
{
    ATLAS_LOGGER_INFO1("Push all telemetry alerts to client with identity ", identity_);

    auto it = pushAlerts_.begin();
    while (it != pushAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry push alert of type ", (*it).first);
        (*it).second->push();
        ++it;
    }
    
    it = thresholdAlerts_.begin();
    while (it != thresholdAlerts_.end()) {
        ATLAS_LOGGER_INFO1("Push to client device telemetry threshold alert of type ", (*it).first);
        (*it).second->push();
        ++it;
    }
}

void AtlasDevice::setIpPort(const std::string &ipPort)
{
    if (ipPort_ != ipPort) {
        ipPort_ = ipPort;
        /* Update the client IP and port to cloud */
        deviceCloud_->updateDevice(identity_, ipPortToJSON());
    }
}

void AtlasDevice::setPolicyInfo(std::unique_ptr<AtlasFirewallPolicy> policy)
{
    if(!policy) {
        ATLAS_LOGGER_ERROR("Received an empty policy!");
        return;
    }

    if((!policy_) || (!(*policy_ == *policy))) {
        policy_ = std::move(policy);
        deviceCloud_->updateDevice(identity_, policy_->toJSON());
    }
}

int AtlasDevice::getRegInterval()
{
    /* If device is registered, then compute the registration time until now */
    if (registered_) {
        /* Get the registration time interval */
        boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - startRegTime_;
        return regIntervalSec_ +  diff.total_seconds();
    }

    return regIntervalSec_;
}

void AtlasDevice::registerNow()
{
    startRegTime_ = boost::posix_time::second_clock::local_time();
    regTime_ = boost::posix_time::to_simple_string(startRegTime_);

    registered_ = true;
    kaCtr_ = ATLAS_KEEP_ALIVE_COUNTER;

    /* Reset all telemetry features to default, the client should provide an update */
    telemetryInfo_.clearFeatures();

    ATLAS_LOGGER_INFO1("Client device registered at ", regTime_);

    /* Update registration event to cloud */
    deviceCloud_->updateDevice(identity_, toJSON());
}

void AtlasDevice::keepAliveNow()
{
    boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
    keepAliveTime_ = boost::posix_time::to_simple_string(timeLocal);

    kaCtr_ = ATLAS_KEEP_ALIVE_COUNTER;
    ATLAS_LOGGER_INFO1("Client device sent a keep-alive at ", keepAliveTime_);

    keepAlivePkts_++;

    /* Update keep-alive event to cloud */
    deviceCloud_->updateDevice(identity_, keepaliveEventToJSON());
}

void AtlasDevice::keepAliveExpired()
{
    if (!kaCtr_)
        return;

    kaCtr_--;
    if (!kaCtr_ && registered_) {
        ATLAS_LOGGER_INFO1("Keep-alive counter expired for client with identity ", identity_);
        /* Get the registration time interval */
        boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - startRegTime_;
        regIntervalSec_ += diff.total_seconds();

        registered_ = false;
        
        uninstallPolicy();
        /* Update un-registration event to cloud */
        deviceCloud_->updateDevice(identity_, registerEventToJSON());
    }
}

void AtlasDevice::syncReputation(AtlasDeviceNetworkType netType)
{
    /* Update the system reputation value to cloud */
    switch (netType) {
        case AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM:
            deviceCloud_->updateDevice(identity_, reputationToJSON(netType));
            break;
        case AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE:
            deviceCloud_->updateDevice(identity_, reputationToJSON(netType));
            break;
        default:
            ATLAS_LOGGER_ERROR("Unknown reputation network type in reputation cloud sync");
    }
}

void AtlasDevice::syncFirewallStatistics()
{
    deviceCloud_->updateDevice(identity_, stats_.toJSON());
}

std::string AtlasDevice::reputationToJSON(AtlasDeviceNetworkType netType)
{   
    std::string tmpRet = "\"";
    switch (netType) {
        case AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM:
            tmpRet = tmpRet + ATLAS_SYSTEM_REPUTATION_JSON_KEY + "\": \"" +
                     std::to_string(deviceReputation_[netType].getReputationScore()) + "\"";
            break;    
        case AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE:
            tmpRet = tmpRet + ATLAS_DATA_REPUTATION_JSON_KEY + "\": \"" +
                     std::to_string(deviceReputation_[netType].getReputationScore()) + "\"";
            break;
        default:
            ATLAS_LOGGER_ERROR("Unknown reputation network type in JSON serialize");
    }
    
    return tmpRet; 
}

std::string AtlasDevice::registerEventToJSON()
{
    std::string regEvent;
    
    if (registered_)
        regEvent = "\"" + ATLAS_REGISTER_JSON_KEY + "\": " + "\"true\",";
    else
        regEvent = "\"" + ATLAS_REGISTER_JSON_KEY + "\": " + "\"false\",";

    regEvent += "\n\"" + ATLAS_LAST_REGISTER_TIME_JSON_KEY + "\": \"" + regTime_ + "\"";
    
    return regEvent;
}

std::string AtlasDevice::keepaliveEventToJSON()
{
    return "\"" + ATLAS_LAST_KEEPALIVE_TIME_JSON_KEY + "\": \"" + keepAliveTime_ + "\"";
}

std::string AtlasDevice::ipPortToJSON()
{
    return "\"" + ATLAS_IP_PORT_JSON_KEY + "\": \"" + ipPort_ + "\"";
}

std::string AtlasDevice::toJSON()
{
    std::string jsonDevice;

    /* Add registration state */
    jsonDevice += registerEventToJSON();
    /* Add keep-alive state */
    jsonDevice += ",\n" + keepaliveEventToJSON();
    /* Add IP and port */
    jsonDevice += ",\n" + ipPortToJSON();
    /* Add telemetry info */
    jsonDevice += ",\n" + telemetryInfo_.toJSON();
    /* Add system reputation */
    jsonDevice += ",\n" + reputationToJSON(AtlasDeviceNetworkType::ATLAS_NETWORK_SYSTEM);
    /* Add data reputation */
    jsonDevice += ",\n" + reputationToJSON(AtlasDeviceNetworkType::ATLAS_NETWORK_SENSOR_TEMPERATURE);
    /* Add firewall statistics info */
    jsonDevice += ",\n" + stats_.toJSON();
    /* Add firewall policy info */
    if (policy_)
        jsonDevice += ",\n" + policy_->toJSON();

    return jsonDevice;
}

std::string AtlasDevice::telemetryInfoToJSON()
{
    return telemetryInfo_.toJSON();
}

void AtlasDevice::setFeature(const std::string &featureType, const std::string &featureValue)
{
    if (featureType == "") {
        ATLAS_LOGGER_ERROR("Invalid feature type");
        return;
    }

    if (telemetryInfo_.getFeature(featureType) != featureValue) {
        ATLAS_LOGGER_INFO("Update cloud with information for feature " + featureType);
        telemetryInfo_.setFeature(featureType, featureValue);
        deviceCloud_->updateDevice(identity_, telemetryInfo_.toJSON(featureType));
    }
}

void AtlasDevice::markCommandAsDone()
{
    ATLAS_LOGGER_INFO("Mark command as done on device with identity " + identity_);

    if (recvCmds_.empty()) {
        ATLAS_LOGGER_ERROR("List with received commands is empty!");
        return;
    }

    /* The value from head is stored into cmdDevice once, and never changed afterwards. */
    const AtlasCommandDevice &cmdDevice = recvCmds_.front();

    bool result = AtlasSQLite::getInstance().markExecutedDeviceCommand(cmdDevice.getSequenceNumber());
    if (!result) {
        ATLAS_LOGGER_ERROR("Cannot update executed status for device command table!");
        return;
    }
   
    /* Transfer the device command from recv to the end of exec container */
    execCmds_.push_back(cmdDevice);
 
    /* Send command DONE status to cloud back-end */
    result = AtlasApprove::getInstance().responseCommandDONE(identity_);
    if (!result)
        ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
 
    /* Removes the first element of the container */
    recvCmds_.pop_front();
}

void AtlasDevice::deviceCmdRespCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_INFO("Command CoAP response for client with identity " + identity_);

    if(recvCmds_.empty()) {
        ATLAS_LOGGER_ERROR("Container recv is empty for client with identity " + identity_);
        return;
    }

    /* If request is successful, the command need to be marked */
    if (respStatus == ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_INFO("Command executed on device with identity " + identity_);
        /* Reset number of timeouts */
        deviceCmdTimeouts_ = 0;
        /* Mark command as DONE (executed by the client) */
        markCommandAsDone(); 
        /* Drain the recv container */
        pushCommand(); 
    } else if (respStatus == ATLAS_COAP_RESP_TIMEOUT) {
        ATLAS_LOGGER_INFO("Command timeout on device with identity " + identity_);
        recvCmds_.front().setInProgress(false);

        if (deviceCmdTimeouts_ == ATLAS_PUSH_COMMAND_RETRY_NO) {
            ATLAS_LOGGER_ERROR("Client is in an error state. Abort resending command");
            /* Reset number of timeouts */
            deviceCmdTimeouts_ = 0;
        } else {
            deviceCmdTimeouts_++;
            /* Try to push again the command to device*/
            pushCommand();
        }
    } else {
        /* If client processed this request and returned an error */
        ATLAS_LOGGER_INFO("Command error on device with identity " + identity_);
        deviceCmdTimeouts_ = 0;

        recvCmds_.front().setInProgress(false);
    } 
}

void AtlasDevice::pushCommand()
{
    if(recvCmds_.empty()) {
        ATLAS_LOGGER_INFO("Empty rcv container for device " + identity_);
        return;
    }
   
    if (!isRegistered()) {
        ATLAS_LOGGER_INFO("Cannot push command for an unregistered device with identity " + identity_);
        return;
    }

    const AtlasCommandDevice &cmdDevice = recvCmds_.front();

    if(cmdDevice.isInProgress()) {
        ATLAS_LOGGER_INFO("The top command [seq no:" +std::to_string(cmdDevice.getSequenceNumber())+ "] from recv container is already on client side! Waitting for reply ...");
        return;
    }

    std::string url = getUrl() + "/" + ATLAS_PUSH_APPROVED_COMMAND;

    ATLAS_LOGGER_DEBUG("Creating command for device");

    /* Set DTLS information for this client device */
    AtlasCoapClient::getInstance().setDtlsInfo(identity_, psk_);

    AtlasCommandBatch cmdBatch;
    std::pair<const uint8_t*, size_t> cmdBuf;

    ATLAS_LOGGER_DEBUG("Sending command to device...");

    /* Add command */
    AtlasCommand cmd(cmdDevice.getCommandTypeDevice(), cmdDevice.getCommandPayloadLength(), cmdDevice.getCommandPayload());

    cmdBatch.addCommand(cmd);
    cmdBuf = cmdBatch.getSerializedAddedCommands();

    try
    {
        /* Send CoAP request */
        coapDeviceCmdToken_ = AtlasCoapClient::getInstance().sendRequest(url, ATLAS_COAP_METHOD_PUT, cmdBuf.first, cmdBuf.second,
                                                                         ATLAS_APPROVED_COMMAND_COAP_TIMEOUT_MS,
                                                                         boost::bind(&AtlasDevice::deviceCmdRespCallback, this, _1, _2, _3));
        recvCmds_.front().setInProgress(true);
    }
    catch(const char *e)
    {
        ATLAS_LOGGER_ERROR(e);
    }
}
AtlasDevice& AtlasDevice::operator= (const AtlasDevice& other)
{
    // check for self-assignment
    if(&other == this)
        return *this;

    identity_ = other.identity_;
    psk_ = other.psk_;
    deviceCloud_ = other.deviceCloud_;
    registered_ = false;
    regIntervalSec_ = 0;
    keepAlivePkts_ = 0;
    coapDeviceCmdToken_ = nullptr;
    deviceCmdTimeouts_ = 0;

    /* Install default alerts */
    installDefaultAlerts();
    /* Init publish-subscribe client id */
    stats_.setClientId(identity_);

    return *this;
}

AtlasDevice::~AtlasDevice()
{
    /* Destroy reference callbacks to this instance*/
    AtlasCoapClient::getInstance().cancelRequest(coapDeviceCmdToken_);
    policy_.reset();
}

} // namespace atlas
