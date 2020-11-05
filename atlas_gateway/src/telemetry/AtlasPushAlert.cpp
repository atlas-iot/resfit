#include <iostream>
#include <boost/bind.hpp>
#include "AtlasPushAlert.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

AtlasPushAlert::AtlasPushAlert(const std::string &deviceIdentity, const std::string &path, uint16_t pushRate): AtlasAlert(deviceIdentity, path)
{
    pushRate_ = pushRate;
}

void AtlasPushAlert::pushCommand(const std::string &url)
{
    AtlasCommandBatch cmdBatch;
    uint16_t pushRate = htons(pushRate_);
    std::pair<const uint8_t*, size_t> cmdBuf;

    ATLAS_LOGGER_DEBUG("Sending push alert to device...");

    /* Add telemetry alert commands */
    AtlasCommand cmdPush(ATLAS_CMD_TELEMETRY_ALERT_EXT_PUSH_RATE, sizeof(uint16_t), (uint8_t *) &pushRate);

    cmdBatch.addCommand(cmdPush);
    cmdBuf = cmdBatch.getSerializedAddedCommands();

    try
    {
        /* Send CoAP request */
        coapToken_ = AtlasCoapClient::getInstance().sendRequest(url, ATLAS_COAP_METHOD_PUT, cmdBuf.first, cmdBuf.second,
                                                            ATLAS_ALERT_COAP_TIMEOUT_MS, boost::bind(&AtlasPushAlert::respCallback, this, _1, _2, _3));
    }
    catch(const char *e)
    {
        ATLAS_LOGGER_ERROR(e);
    }
}

} // namespace atlas

