#include <string>
#include <iostream>
#include <boost/optional/optional_io.hpp>
#include "AtlasFeatureReputation.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../mosquitto_plugin/filter/AtlasFilter.h"

namespace atlas {

const std::string ATLAS_FEATURE_RECEIVE_URI = "gateway/reputation/feature";
const std::string ATLAS_FEEDBACK_RECEIVE_URI = "gateway/reputation/feedback";

AtlasFeatureReputation::AtlasFeatureReputation() : featureReputationResource_(ATLAS_FEATURE_RECEIVE_URI,
                                                                              ATLAS_COAP_METHOD_PUT,
                                                                              boost::bind(&AtlasFeatureReputation::requestReputationCallback,
                                                                                          this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                                    receiveFeedbackResource_(ATLAS_FEEDBACK_RECEIVE_URI,
                                                                             ATLAS_COAP_METHOD_PUT,
                                                                             boost::bind(&AtlasFeatureReputation::feedbackReputationCallback,
                                                                                         this, _1, _2, _3, _4, _5, _6, _7, _8)) {}

AtlasCoapResponse AtlasFeatureReputation::requestReputationCallback(const std::string &path, const std::string &pskIdentity,
                                                                    const std::string &psk, AtlasCoapMethod method,
                                                                    const uint8_t* reqPayload, size_t reqPayloadLen,
                                                                    uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    AtlasCommandBatch cmdRespBatch;
    std::vector<AtlasCommand> cmd;
    std::pair<const uint8_t*, size_t> cmdBuf;
    std::string identity;
    boost::optional<uint16_t> sensorType;
    uint16_t tmp;
    uint8_t *buf = nullptr;
    
    ATLAS_LOGGER_DEBUG("Feature callback executed...");

    ATLAS_LOGGER_INFO1("Process FEATURE command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice* device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + pskIdentity);
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    if (!device->isRegistered()) {
        ATLAS_LOGGER_ERROR("Received FEATURE command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse command */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Feature end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Feature end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Feature end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } else if(cmdEntry.getType() == ATLAS_CMD_FEATURE_REQUEST) {
            if (cmdEntry.getLen() != sizeof(tmp)) {
                ATLAS_LOGGER_ERROR("Feature end-point called with empty FEATURE command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            memcpy(&tmp, cmdEntry.getVal(), sizeof(tmp));
            tmp = ntohs(tmp);
            sensorType = tmp;
        }
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Feature reputation failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (!sensorType) {
        ATLAS_LOGGER_ERROR("Feature reputation failed because of invalid sensor type");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    ATLAS_LOGGER_INFO("Device with identity " + identity + " sent a reputation request for feature " + std::to_string(*sensorType));
    
    std::string mostTrusted = AtlasDeviceManager::getInstance().getTrustedDevice((AtlasDeviceNetworkType) *sensorType);
    if (mostTrusted == "" || mostTrusted == identity) {
        ATLAS_LOGGER_INFO("Cannot find the most trusted device for feature" + std::to_string(*sensorType));
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Add feature command */
    AtlasCommand cmdPush(ATLAS_CMD_FEATURE_REQUEST, mostTrusted.size(), (uint8_t *) mostTrusted.c_str());
    cmdBatch.addCommand(cmdPush);

    /* Serialize response */
    cmdBuf = cmdBatch.getSerializedAddedCommands();
    buf = new uint8_t[cmdBuf.second];
    memcpy(buf, cmdBuf.first, cmdBuf.second);

    /* Set CoAP response */
    *respPayload = buf;
    *respPayloadLen = cmdBuf.second;
 
    return ATLAS_COAP_RESP_OK;
}

AtlasCoapResponse AtlasFeatureReputation::feedbackReputationCallback(const std::string &path, const std::string &pskIdentity,
                                                                     const std::string &psk, AtlasCoapMethod method,
                                                                     const uint8_t* reqPayload, size_t reqPayloadLen,
                                                                     uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    AtlasCommandBatch cmdInnerBatch;
    std::vector<AtlasCommand> cmd;
    std::vector<AtlasCommand> cmdInner;
    std::pair<const uint8_t*, size_t> cmdBuf;
    std::string identity;
    std::string feedbackIdentity;
    boost::optional<uint16_t> sensorType;
    bool sensorScore = false;
    bool respTimeScore = false;
    double value;
    uint16_t tmp;
    std::vector<std::pair<AtlasDeviceFeatureType, double>> feedbackMatrix;
    
    ATLAS_LOGGER_DEBUG("Feedback callback executed...");

    ATLAS_LOGGER_INFO1("Process FEEDBACK command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice* device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + pskIdentity);
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    
    if (!device->isRegistered()) {
        ATLAS_LOGGER_ERROR("Received FEEDBACK command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse command */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Feedback end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Feedback end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Feedback end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } else if(cmdEntry.getType() == ATLAS_CMD_FEEDBACK) {
            /* Parse feedback command */
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Empty FEEDBACK command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            cmdInnerBatch.setRawCommands(cmdEntry.getVal(), cmdEntry.getLen());
            cmdInner = cmdInnerBatch.getParsedCommands();

            for (AtlasCommand &cmdInnerEntry : cmdInner) {
                if (cmdInnerEntry.getType() == ATLAS_CMD_FEEDBACK_IDENTITY)
                    feedbackIdentity.assign((char *)cmdInnerEntry.getVal(), cmdInnerEntry.getLen());
                else if (cmdInnerEntry.getType() == ATLAS_CMD_FEEDBACK_SENSOR_TYPE) {
                    memcpy(&tmp, cmdInnerEntry.getVal(), cmdInnerEntry.getLen());
                    tmp = ntohs(tmp);
                    sensorType = tmp;
                } else if (cmdInnerEntry.getType() == ATLAS_CMD_FEEDBACK_SENSOR) {
                    memcpy(&tmp, cmdInnerEntry.getVal(), cmdInnerEntry.getLen());
                    tmp = ntohs(tmp);
                    value = (double) tmp / 100;
                    feedbackMatrix.push_back(std::pair<AtlasDeviceFeatureType, double>(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_SENSOR, value));
                    sensorScore = true;
                } else if (cmdInnerEntry.getType() == ATLAS_CMD_FEEDBACK_RESPONSE_TIME) {
                    memcpy(&tmp, cmdInnerEntry.getVal(), cmdInnerEntry.getLen());
                    tmp = ntohs(tmp);
                    value = (double) tmp / 100;
                    feedbackMatrix.push_back(std::pair<AtlasDeviceFeatureType, double>(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_RESP_TIME, value));
                    respTimeScore = true;
                }
            }
        }
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Receive feedback failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (feedbackIdentity == "") {
        ATLAS_LOGGER_ERROR("Received feedback failed because of invalid feedback identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (!sensorType) {
        ATLAS_LOGGER_ERROR("Received feedback failed because of invalid feedback sensor type");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (!sensorScore) {
        ATLAS_LOGGER_ERROR("Received feedback failed because of invalid feedback sensor score");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (!respTimeScore) {
        ATLAS_LOGGER_ERROR("Received feedback failed because of invalid feedback response time score");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    ATLAS_LOGGER_INFO("Device with identity " + identity + " send a feedback value for device with identity " + feedbackIdentity);

    AtlasDeviceManager::getInstance().updateDataReputation(feedbackIdentity,
                                                           (AtlasDeviceNetworkType) *sensorType,
                                                           feedbackMatrix);
 
    return ATLAS_COAP_RESP_OK;
}

void AtlasFeatureReputation::start()
{
    ATLAS_LOGGER_DEBUG("Start FEATURE Reputation module");

    /* Add CoAP resource for reputation feature */
    AtlasCoapServer::getInstance().addResource(featureReputationResource_);
    AtlasCoapServer::getInstance().addResource(receiveFeedbackResource_);
}

void AtlasFeatureReputation::stop()
{
    ATLAS_LOGGER_DEBUG("Stop FEATURE reputation module");

    /* Remove CoAP resource for reputation feature */
    AtlasCoapServer::getInstance().delResource(featureReputationResource_);
    AtlasCoapServer::getInstance().delResource(receiveFeedbackResource_);
}

} // namespace atlas
