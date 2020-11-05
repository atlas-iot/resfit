#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include "AtlasRegister.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

namespace {

const std::string ATLAS_REGISTER_URI = "gateway/register";
const std::string ATLAS_KEEPALIVE_URI = "gateway/keepalive";

} // anonymous namespace

AtlasRegister::AtlasRegister() : registerResource_(ATLAS_REGISTER_URI,
                                                   ATLAS_COAP_METHOD_POST,
                                                   boost::bind(&AtlasRegister::registerCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                 keepAliveResource_(ATLAS_KEEPALIVE_URI,
                                                    ATLAS_COAP_METHOD_PUT,
                                                    boost::bind(&AtlasRegister::keepaliveCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                 kaAlarm_("AtlasRegisterKeepAlive", ATLAS_KEEP_ALIVE_INTERVAL_MS,
                                          false, boost::bind(&AtlasRegister::keepaliveAlarmCallback, this)) {}

void AtlasRegister::keepaliveAlarmCallback()
{
    ATLAS_LOGGER_INFO("Keep-alive alarm callback");

    AtlasDeviceManager::getInstance().forEachDevice([] (AtlasDevice& device)
                                                        { 
                                                            if (device.isRegistered())
                                                                device.keepAliveExpired();
                                                        });
}

AtlasCoapResponse AtlasRegister::keepaliveCallback(const std::string &path, const std::string &pskIdentity,
                                                   const std::string& psk, AtlasCoapMethod method,
                                                   const uint8_t* reqPayload, size_t reqPayloadLen,
                                                   uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity = "";
    std::string ipPort = "";
    uint8_t *token = nullptr;
    
    ATLAS_LOGGER_DEBUG("Keepalive callback executed...");

    ATLAS_LOGGER_INFO1("Process KEEPALIVE command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice* device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + pskIdentity);
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    if (!device->isRegistered()) {
        ATLAS_LOGGER_ERROR("Received KEEPALIVE command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    
    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Keep-alive end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_KEEPALIVE) {
            ATLAS_LOGGER_DEBUG("Keep-alive end-point called and KEEPALIVE command is found");
           
            /* The keep-alive token must have two bytes length */
            if (cmdEntry.getLen() != sizeof(uint16_t)) {
                ATLAS_LOGGER_ERROR("Keep-alive end-point called with invalid KEEPALIVE token length");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            token = new uint8_t[cmdEntry.getLen()];
            memcpy(token, cmdEntry.getVal(), cmdEntry.getLen());
        } else if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            ATLAS_LOGGER_DEBUG("Keep-alive end-point called and IDENTITY command is found");
           
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Keep-alive end-point called with empty IDENTITY command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Keep-alive end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } else if (cmdEntry.getType() == ATLAS_CMD_IP_PORT) {
            ATLAS_LOGGER_DEBUG("Registration end-point called and UPDATE IP_PORT command is found");
           
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Registration end-point called with empty UPDATE IP_PORT command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            ipPort.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
        }

    }

    if (identity == "" || !token || ipPort == "") {
        ATLAS_LOGGER_ERROR("Keep-alive failed because of invalid identity, keep-alive token or ip_port address");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Echo keep-alive token */
    *respPayload = token;
    *respPayloadLen = sizeof(uint16_t);

    ATLAS_LOGGER_INFO1("Keep-alive SUCCESS sent by client with identity ", identity);

    /* Notify device that a keep-alive was just received */
    device->keepAliveNow();

    /* Save ip address in device client*/
    device->setIpPort(ipPort);
    
    return ATLAS_COAP_RESP_OK;
}

AtlasCoapResponse AtlasRegister::registerCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string &psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity = "";
    std::string ipPort = "";
    
    ATLAS_LOGGER_DEBUG("Register callback executed...");

    ATLAS_LOGGER_INFO1("Process REGISTER command from client with DTLS PSK identity ", pskIdentity);

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Registration end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_REGISTER) {
            ATLAS_LOGGER_DEBUG("Registration end-point called and REGISTER command is found");
           
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Registration end-point called with empty REGISTER command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Register end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } else if (cmdEntry.getType() == ATLAS_CMD_IP_PORT) {
            ATLAS_LOGGER_DEBUG("Registration end-point called and UPDATE IP_PORT command is found");
           
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Registration end-point called with empty UPDATE IP_PORT command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            ipPort.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
        } 
    }

    if (identity == "" || ipPort == "") {
        ATLAS_LOGGER_ERROR("Registration end-point called with no REGISTER commands");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    ATLAS_LOGGER_INFO1("New ATLAS client is trying to register with identity ", identity);

    AtlasDevice* device = AtlasDeviceManager::getInstance().getDevice(identity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + pskIdentity);
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    
    /* Create device (if necessary) and set IP */
    device->registerNow();
    device->setIpPort(ipPort);

    /* Install alerts on client device */
    device->pushAlerts();

    return ATLAS_COAP_RESP_OK;
}

void AtlasRegister::start()
{
    ATLAS_LOGGER_DEBUG("Start registration module");

    /* Add REGISTER and KEEPALIVE resource to CoAP server. This is called by the IoT device in the 
       registration phase and for keepalive pings after that.*/
    AtlasCoapServer::getInstance().addResource(registerResource_);
    AtlasCoapServer::getInstance().addResource(keepAliveResource_);

    /* Start keep-alive alarm */
    kaAlarm_.start();
}

void AtlasRegister::stop()
{
    ATLAS_LOGGER_DEBUG("Stop registration module");
    
    /* Unregister CoAP resources */
    AtlasCoapServer::getInstance().delResource(registerResource_);
    AtlasCoapServer::getInstance().delResource(keepAliveResource_);

    /* Stop keep-alive alarm */
    kaAlarm_.cancel();
}

} // namespace atlas
