#include <iostream>
#include <boost/bind.hpp>
#include "AtlasThresholdAlert.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

AtlasThresholdAlert::AtlasThresholdAlert(const std::string &deviceIdentity, const std::string &path,
                                         uint16_t scanRate, const std::string &threshold): AtlasAlert(deviceIdentity, path)
{
    scanRate_ = scanRate;
    threshold_ = threshold;
}

void AtlasThresholdAlert::pushCommand(const std::string &url)
{
    AtlasCommandBatch cmdBatch;
    uint16_t scanRate = htons(scanRate_);
    std::pair<const uint8_t*, size_t> cmdBuf;

    ATLAS_LOGGER_DEBUG("Sending threshold alert to device...");

    /* Add telemetry alert commands */
    AtlasCommand cmdIntScan(ATLAS_CMD_TELEMETRY_ALERT_INT_SCAN_RATE, sizeof(uint16_t), (uint8_t *) &scanRate);
    AtlasCommand cmdThreshold(ATLAS_CMD_TELEMETRY_ALERT_THRESHOLD, threshold_.length(), (uint8_t *) threshold_.c_str());

    cmdBatch.addCommand(cmdIntScan);
    cmdBatch.addCommand(cmdThreshold);
    cmdBuf = cmdBatch.getSerializedAddedCommands();

    try
    {
        /* Send CoAP request */
        coapToken_ = AtlasCoapClient::getInstance().sendRequest(url, ATLAS_COAP_METHOD_PUT, cmdBuf.first, cmdBuf.second,
                                                ATLAS_ALERT_COAP_TIMEOUT_MS, boost::bind(&AtlasThresholdAlert::respCallback, this, _1, _2, _3));
    }
    catch(const char *e)
    {
        ATLAS_LOGGER_ERROR(e);
    }
}

} // namespace atlas

