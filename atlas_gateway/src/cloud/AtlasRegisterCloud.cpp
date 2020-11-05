#include <boost/bind.hpp>
#include "AtlasRegisterCloud.h"
#include "AtlasCommandsCloud.h"
#include "../logger/AtlasLogger.h"
#include "../mqtt_client/AtlasMqttClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../identity/AtlasIdentity.h"

namespace atlas {

namespace {

const int ATLAS_KEEPALIVE_CLOUD_INTERVAL_MS = 60000;

} // anonymous namespace

AtlasRegisterCloud::AtlasRegisterCloud() : registered_(false),
                                           kaAlarm_("AtlasRegisterCloud", ATLAS_KEEPALIVE_CLOUD_INTERVAL_MS,
                                                    false, boost::bind(&AtlasRegisterCloud::keepaliveAlarmCb, this)) {}

AtlasRegisterCloud& AtlasRegisterCloud::getInstance()
{
    static AtlasRegisterCloud instance;

    return instance;
}

void AtlasRegisterCloud::sendRegisterCmd()
{
    std::string cmd = "{\n";
    
    ATLAS_LOGGER_INFO("Send REGISTER command to cloud back-end");

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_GATEWAY_REGISTER + "\"";
    cmd += "\n}";

    /* Send to REGISTER command */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, cmd);
    if (delivered) {
        ATLAS_LOGGER_INFO("REGISTER command was sent to cloud back-end. Sync all devices...");
        registered_ = true;
        AtlasDeviceManager::getInstance().getDeviceCloud()->allDevicesUpdate();
        AtlasDeviceManager::getInstance().getDeviceCloud()->gatewayFullUpdate();
    }
}

void AtlasRegisterCloud::sendKeepaliveCmd()
{
    std::string cmd = "{\n";
    
    ATLAS_LOGGER_INFO("Send KEEPALIVE command to cloud back-end");

    /* Add header */
    cmd += "\"" + ATLAS_CMD_TYPE_JSON_KEY + "\": \"" + ATLAS_CMD_GATEWAY_KEEPALIVE + "\"";
    cmd += "\n}";

    /* Send to KEEPALIVE command */
    bool delivered = AtlasMqttClient::getInstance().tryPublishMessage(AtlasIdentity::getInstance().getPsk() + ATLAS_TO_CLOUD_TOPIC, cmd);
    if (!delivered)
        ATLAS_LOGGER_ERROR("KEEPALIVE command was not sent to cloud back-end");
}

void AtlasRegisterCloud::keepaliveAlarmCb()
{
    ATLAS_LOGGER_INFO("Cloud keep-alive alarm triggered");

    if (!registered_)
        sendRegisterCmd();
    else
        sendKeepaliveCmd();
}

void AtlasRegisterCloud::start()
{
    ATLAS_LOGGER_DEBUG("Start cloud register module");

    kaAlarm_.start();

    AtlasMqttClient::getInstance().addConnectionCb(this);
}

void AtlasRegisterCloud::stop()
{
    ATLAS_LOGGER_DEBUG("Stop cloud register module");

    AtlasMqttClient::getInstance().removeConnectionCb(this);

    kaAlarm_.cancel();
}

void AtlasRegisterCloud::onConnect()
{
    ATLAS_LOGGER_INFO("Connect event to cloud MQTT broker");

    if (!registered_)
        sendRegisterCmd();
}

void AtlasRegisterCloud::onDisconnect()
{
    ATLAS_LOGGER_INFO("Disconnect event from cloud MQTT broker");

    registered_ = false;
}

} // namespace atlas
