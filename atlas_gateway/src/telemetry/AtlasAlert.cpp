#include <iostream>
#include <boost/bind.hpp>
#include "AtlasAlert.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandBatch.h"
#include "../commands/AtlasCommandType.h"

namespace atlas {

AtlasAlert::AtlasAlert(const std::string &deviceIdentity, const std::string &path):coapToken_(nullptr)
{
    deviceIdentity_ = deviceIdentity;
    path_ = path;
}

void AtlasAlert::respCallback(AtlasCoapResponse respStatus, const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_INFO1("Telemetry alert CoAP response for client with identity ", deviceIdentity_);

    /* If request is successful, there is nothing to do */
    if (respStatus == ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_INFO1("Telemetry alert installed for device with identity ", deviceIdentity_);
        return;
    }

    /* If client processed this request and returned an error, then skip it */
    if (respStatus != ATLAS_COAP_RESP_TIMEOUT) {
        ATLAS_LOGGER_INFO("Telemetry alert URI error on the client side. Abording request...");
        return;
    }

    /* Try to push again the telemetry alert to device */
    push();
}

void AtlasAlert::push()
{
    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(deviceIdentity_);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + deviceIdentity_);
        return;
    }
    
    std::string url = device->getUrl() + "/" + path_;

    ATLAS_LOGGER_DEBUG("Creating command for telemetry alert push");

    if (!device->isRegistered()) {
        ATLAS_LOGGER_INFO1("Cannot push telemetry alert for OFFLINE device with identity ", deviceIdentity_);
        return;
    }

    /* Set DTLS information for this client device */
    AtlasCoapClient::getInstance().setDtlsInfo(deviceIdentity_, device->getPsk());

    pushCommand(url);
}

AtlasAlert::~AtlasAlert()
{
    /* Destroy reference callbacks to this instance*/
    AtlasCoapClient::getInstance().cancelRequest(coapToken_);
}

} // namespace atlas

