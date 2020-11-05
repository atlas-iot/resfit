#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include "AtlasPolicy.h"
#include "../coap/AtlasCoapServer.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"
#include "../mosquitto_plugin/filter/AtlasFilter.h"
#include "../pubsub_agent/AtlasPubSubAgent.h"
#include "AtlasFirewallPolicy.h"

namespace atlas {

const std::string ATLAS_FIREWALLPOLICY_URI = "gateway/policy";

AtlasPolicy::AtlasPolicy() : firewallPolicyResource_(ATLAS_FIREWALLPOLICY_URI,
                                                    ATLAS_COAP_METHOD_PUT,
                                                    boost::bind(&AtlasPolicy::firewallPolicyCallback, this, _1, _2, _3, _4, _5, _6, _7, _8)) {}


AtlasCoapResponse AtlasPolicy::firewallPolicyCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string &psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    uint16_t tmp;
    boost::optional<uint16_t> qos;
    boost::optional<uint16_t> ppm;
    boost::optional<uint16_t> payloadLen;

    ATLAS_LOGGER_DEBUG("Firewall policy callback executed...");

    ATLAS_LOGGER_INFO1("Process FIREWALL POLICY INSTALL command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice* device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + pskIdentity);
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    if (!device->isRegistered()) {
        ATLAS_LOGGER_ERROR("Received FIREWALL POLICY INSTALL command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Firewall policy end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_MAX_QOS) {
            ATLAS_LOGGER_DEBUG("Policy end-point called and QOS command is found");
            if (cmdEntry.getLen() != sizeof(uint16_t)) {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty QOS command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            memcpy(&tmp, cmdEntry.getVal(), cmdEntry.getLen());
	    qos = ntohs(tmp);
        } else if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_PER_MINUTE) {
            ATLAS_LOGGER_DEBUG("Policy end-point called and PPM command is found");
            if (cmdEntry.getLen() != sizeof(uint16_t)) {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty PPM command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            memcpy(&tmp, cmdEntry.getVal(), cmdEntry.getLen());
	    ppm = ntohs(tmp);
        } else if (cmdEntry.getType() == ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_MAXLEN) {
            ATLAS_LOGGER_DEBUG("Policy end-point called and PAYLOAD_LEN command is found");
            if (cmdEntry.getLen() != sizeof(uint16_t)) {
                ATLAS_LOGGER_ERROR("Policy end-point called with empty PAYLOAD_LEN command");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }

            memcpy(&tmp, cmdEntry.getVal(), cmdEntry.getLen());
	    payloadLen = ntohs(tmp);
        }
    }

    if (!qos || !ppm || !payloadLen) {
        ATLAS_LOGGER_ERROR("Policy failed because of invalid params");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    std::unique_ptr<AtlasFirewallPolicy> policyAux = std::unique_ptr<AtlasFirewallPolicy>(new AtlasFirewallPolicy());

    policyAux->setClientId(pskIdentity);
    policyAux->setQOS(*qos);
    policyAux->setPPM(*ppm);
    policyAux->setPayloadLen(*payloadLen);

    /* Set policy info in this device */
    device->setPolicyInfo(std::move(policyAux));

    /* Install firewall policy in mosquitto plug-in */
    AtlasPubSubAgent::getInstance().installFirewallRule(pskIdentity, device->getPolicy());

    return ATLAS_COAP_RESP_OK;
}


void AtlasPolicy::start()
{

    ATLAS_LOGGER_DEBUG("Start policy module");

    AtlasCoapServer::getInstance().addResource(firewallPolicyResource_);
}

void AtlasPolicy::stop()
{
    ATLAS_LOGGER_DEBUG("Stop policy module");
    
    AtlasCoapServer::getInstance().delResource(firewallPolicyResource_);
}

} // namespace atlas
