#include "AtlasApprove.h"
#include "AtlasClaim.h"
#include "../sql/AtlasSQLite.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../device/AtlasDevice.h"
#include "../cloud/AtlasCommandsCloud.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../identity/AtlasIdentity.h"
#include "../utils/AtlasUtils.h"

namespace atlas {

namespace {

const std::string ATLAS_CLAIM_REQUEST_PATH = "/gateway/claim";

/* JSON fields from the incoming approve messages */
const std::string ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY    = "clientIdentity";
const std::string ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY      = "type";
const std::string ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY   = "payload";
const std::string ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY       = "seqNo";
const std::string ATLAS_CMD_PAYLOAD_SIGNATURE_JSON_KEY = "signature";

/* Check container at every 30 sec */
const int ATLAS_PUSH_COMMAND_ALARM_PERIOD_MS = 30000;

/* Resend status at each 10 sec */
const int ATLAS_STATUS_COMMAND_ALARM_PERIOD_MS = 10000; 

/* Retry sending status for 3 times */
const uint8_t ATLAS_STATUS_COMMAND_RETRY_NO = 3; 

} // anonymous namespace

AtlasApprove::AtlasApprove(): pushCommandAlarm_("AtlasApprovePushCommand", ATLAS_PUSH_COMMAND_ALARM_PERIOD_MS, false,
                                                boost::bind(&AtlasApprove::alarmCallback, this)),
                              statusACKAlarm_("AtlasApproveACK", ATLAS_STATUS_COMMAND_ALARM_PERIOD_MS, false,
                                                boost::bind(&AtlasApprove::statusACKCallback, this)),
                              msgACKScheduled_(false), counterACK_(0) {}

AtlasApprove& AtlasApprove::getInstance()
{
    static AtlasApprove instance;

    return instance;
}

void AtlasApprove::start()
{
    ATLAS_LOGGER_DEBUG("Start device approved command module");
    sequenceNumber_ = 0;

    bool result = AtlasSQLite::getInstance().getMaxSequenceNumber();
    if(result) {

        ATLAS_LOGGER_INFO("Initialize sequence number from database!");
    } else {

        ATLAS_LOGGER_INFO("Uncommited select on device commands in getMaxSequenceNumber function");
        
    }
    pushCommandAlarm_.start();
}


void AtlasApprove::stop()
{
    ATLAS_LOGGER_DEBUG("Stop device approved command module");
    pushCommandAlarm_.cancel();
}

void AtlasApprove::alarmCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for pushing device commands to clients");

    /* Push top command for each device */
    AtlasDeviceManager::getInstance().forEachDevice([] (AtlasDevice& device)
                                                    {
                                                        /* Event for sending device commands to client*/ 
                                                        device.pushCommand();

                                                        /* Event for sending DONE notification for executed device commands to cloud (got notified by client in a scheduled DONE window) */
                                                        if (device.isExecCommandAvailable()) {
                                                            const AtlasCommandDevice &cmd = device.getExecutedCommand();
                                                            bool result = AtlasApprove::getInstance().responseCommandDONE(cmd.getDeviceIdentity());
                                                            if(!result) {
                                                                ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
                                                                return;
                                                            }
                                                        }
                                                    });
                                
}

void AtlasApprove::statusACKCallback()
{
    ATLAS_LOGGER_DEBUG("Execute alarm for sending ACK status device commands to cloud");

    bool result = responseCommandACK();
    if(!result) {
        ATLAS_LOGGER_INFO("Retry ACK message " + std::to_string(ATLAS_STATUS_COMMAND_RETRY_NO - counterACK_) + " times");
    } else {
        ATLAS_LOGGER_INFO("Stop scheduling ACK message");
        statusACKAlarm_.cancel();
        msgACKScheduled_ = false;
        counterACK_ = 0;
    }

    return;
}

bool AtlasApprove::handleOldCommand(const Json::Value &payload)
{
    /*  Check if the received command is already in db 
    This is a case when the back-end cloud did not recevied the ACK for first sending of this command */

    bool result = AtlasSQLite::getInstance().checkDeviceCommandBySeqNo(payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());
    if (!result)
        return false;
       
    sequenceNumber_ = payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
    result = AtlasSQLite::getInstance().checkDeviceCommandForExecution(payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt());

    /* If command is already executed */
    if(result) {
        /* Send directly DONE status if the command is already executed and the cloud did not received the ACK status until now*/
        result = responseCommandDONE(payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());

        if(!result) {
            ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
            return false;
        }

        return true;

    } else {
        /* Command is not executed by client, but was previously acknowledged by gateway */
        result = responseCommandACK();
        if (!result) {
            ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
            return false;
        }

        return true;
 
    }
    
    return false;
}

bool AtlasApprove::handleClientCommand(const Json::Value &payload)
{
    /* Sanity checks */
    if (payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty sequence number field!");
        return false;
    }
    if (payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty device id field!");
        return false;
    }
    if (payload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString() == "") {
        ATLAS_LOGGER_ERROR("Received a command with an empty command type field!");
        return false;
    }

    if (AtlasDeviceManager::getInstance().getGateway().isClaimed()) {
        ATLAS_LOGGER_INFO("Received an approved command for a claimed gateway!");
        
        if (!validateCommandSignature(payload)) {
            ATLAS_LOGGER_ERROR("The received data was changed in transit!");
            return false;
        }
    } else {
        /* If gateway is unclaimed, the received command does not need to be authorized*/
        ATLAS_LOGGER_INFO("Received a command for an unclaimed gateway!");
    }

    /* Handle commands with an old sequence number */
    if (payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt() <= sequenceNumber_) {
        ATLAS_LOGGER_ERROR("Received a command in which the sequence number is less or equal than current sequence number!");

        return handleOldCommand(payload);
    } 
    
    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if (!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
        return false;
    }

    /* Save command into the database */
    bool result = AtlasSQLite::getInstance().insertDeviceCommand(payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                                                                 payload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                                                                 payload[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString(),
                                                                 payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if (!result) {
        ATLAS_LOGGER_ERROR("Cannot save device command into the database!");
	    return false;
    }

    /* Insert command into received device commands container (list) */
    AtlasCommandDevice cmd(payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString(),
                           payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt(),
                           payload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString(),
                           payload[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString());

    device->addRecvDeviceCommand(std::move(cmd));

    sequenceNumber_ = payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt();
    result = responseCommandACK();
    if (!result) {
        ATLAS_LOGGER_ERROR("ACK message was not sent to cloud back-end");
        return false;
    }
    
    /* Send the device command to client as soon as I received it */
    device->pushCommand();

    return true;
}

bool AtlasApprove::responseCommandACK()
{
    Json::FastWriter fastWriter;
    Json::Value cmd;
    
    /* Set command type */
    cmd[ATLAS_CMD_TYPE_JSON_KEY] = ATLAS_CMD_GATEWAY_CLIENT_ACK;
    /* Set command payload (sequence number) */
    cmd[ATLAS_CMD_PAYLOAD_JSON_KEY][ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY] = sequenceNumber_;

    std::string ackCmd = fastWriter.write(cmd);
    
    ATLAS_LOGGER_INFO("Send ACK response to cloud back-end for command with sequence number " + std::to_string(sequenceNumber_));

    /* Send ACK message */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, ackCmd);
    
    /* If message is not delivered, then schedule an ACK message */
    if (!delivered) {
        ATLAS_LOGGER_ERROR("ACK message was not sent to cloud back-end");

        if (!msgACKScheduled_) {
            ATLAS_LOGGER_INFO("Schedule ACK message");
            statusACKAlarm_.start();
            msgACKScheduled_ = true;
        }
        
        if(counterACK_ == ATLAS_STATUS_COMMAND_RETRY_NO) {
            ATLAS_LOGGER_INFO("Stop scheduling ACK message");
            statusACKAlarm_.cancel();
            msgACKScheduled_ = false;
            counterACK_ = 0;
        }

        counterACK_++;

    } else {
        ATLAS_LOGGER_INFO("ACK message was sent to cloud back-end");
    }

    return delivered;
}

bool AtlasApprove::responseCommandDONE(const std::string &deviceIdentity)
{
    Json::FastWriter fastWriter;
    Json::Value cmd;

    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(deviceIdentity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + deviceIdentity);
        return false;
    }
    const AtlasCommandDevice &cmdDevice = device->getExecutedCommand();

    ATLAS_LOGGER_INFO("Send DONE response to cloud back-end");
    
    /* Set command type */
    cmd[ATLAS_CMD_TYPE_JSON_KEY] = ATLAS_CMD_GATEWAY_CLIENT_DONE;
    /* Set command payload */
    cmd[ATLAS_CMD_PAYLOAD_JSON_KEY][ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY] = cmdDevice.getSequenceNumber();
    /* Set client identity */
    cmd[ATLAS_CMD_PAYLOAD_JSON_KEY][ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY] = deviceIdentity;

    std::string doneCmd = fastWriter.write(cmd);

    /* Send DONE message */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, doneCmd);
    if (!delivered) {
        ATLAS_LOGGER_ERROR("DONE message was not sent to cloud back-end");
    } else {
        ATLAS_LOGGER_INFO("DONE message was sent to cloud back-end");
    }

    return delivered;
}

void AtlasApprove::handleCommandDoneAck(const Json::Value &payload)
{
    ATLAS_LOGGER_INFO("Handle ACK message for client command");

    /* the command payload contains only the client identity */
    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists with identity " + payload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString());
        return;
    }

    if (!device->isExecCommandAvailable()) {
        ATLAS_LOGGER_ERROR("ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND: container of executed device commands is empty");
        return;
    }

    const AtlasCommandDevice &cmd = device->getExecutedCommand();    
    if (cmd.getSequenceNumber() != payload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asUInt()) {
        ATLAS_LOGGER_ERROR("Client command sequence number mismatch when processing ACK message for a DONE command");
        return;
    }

    ATLAS_LOGGER_INFO("Handle ACK message for DONE command with sequence number: "
                      + std::to_string(cmd.getSequenceNumber()) + " for client device with identity: " + device->getIdentity());

    bool result = AtlasSQLite::getInstance().deleteDeviceCommand(cmd.getSequenceNumber());
    if(!result) {
        ATLAS_LOGGER_ERROR("ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND error on delete a device command in database");
        return;
    }

    device->removeExecutedCommand();
    /* send additional commands in the done list (drain list)! */
    responseCommandDONE(device->getIdentity());
}

bool AtlasApprove::validateCommandSignature(const Json::Value &commandPayload)
{
    std::vector<uint8_t> commandSignature;

    /* If gateway is claimed, the received command need to be authorized(HMAC)
       HMAC SHA512: (gatewayId + clientId + clientCommandType + clientCommandPayload + seqNo) */

    std::vector<uint8_t> key = base64::decode(AtlasDeviceManager::getInstance().getGateway().getOwnerSecretKey());
    std::string payload = AtlasIdentity::getInstance().getIdentity()
                          + commandPayload[ATLAS_CMD_PAYLOAD_CLIENT_JSON_KEY].asString()
                          + commandPayload[ATLAS_CMD_PAYLOAD_TYPE_JSON_KEY].asString()
                          + commandPayload[ATLAS_CMD_PAYLOAD_PAYLOAD_JSON_KEY].asString()
                          + commandPayload[ATLAS_CMD_PAYLOAD_SEQ_JSON_KEY].asString();

    std::vector<uint8_t> hmacResult = crypto::hmacSHA512(key, std::vector<uint8_t>(reinterpret_cast<const uint8_t*>(payload.c_str()),
                                                                          reinterpret_cast<const uint8_t*>(payload.c_str()) + payload.length()));
    
    try {
        commandSignature = base64::decode(commandPayload[ATLAS_CMD_PAYLOAD_SIGNATURE_JSON_KEY].asString());
    } catch(...) {
        ATLAS_LOGGER_ERROR("An exception occured while trying to decode the command signature");
        return false;
    }

    return hmacResult == commandSignature;
}

} // namespace atlas
