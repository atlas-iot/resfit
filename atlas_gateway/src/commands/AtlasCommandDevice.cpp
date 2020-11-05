
#include <iostream>
#include <boost/bind.hpp>
#include "AtlasCommandDevice.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapClient.h"
#include "AtlasCommandBatch.h"
#include "AtlasCommandType.h"
#include "../device/AtlasDeviceManager.h"
#include "../claim_approve/AtlasApprove.h"
#include "../sql/AtlasSQLite.h"

namespace atlas {

namespace {

const std::string ATLAS_CMD_DEVICE_RESTART_CLOUD = "ATLAS_CMD_CLIENT_DEVICE_RESTART";
const std::string ATLAS_CMD_DEVICE_SHUTDOWN_CLOUD = "ATLAS_CMD_CLIENT_DEVICE_SHUTDOWN";

} // anonymous namespace


AtlasCommandDevice::AtlasCommandDevice( const std::string &deviceIdentity, const uint32_t sequenceNumber, 
                                        const std::string &commandType, const std::string &commandPayload): 
                                        deviceIdentity_(deviceIdentity), sequenceNumber_(sequenceNumber), 
                                        commandTypeCloud_(commandType), commandPayload_(commandPayload),
                                        inProgress(false)
{
    if(commandType == ATLAS_CMD_DEVICE_RESTART_CLOUD)
        commandTypeDevice_ = AtlasCommandDeviceType::ATLAS_CMD_DEVICE_RESTART;
    else if(commandType == ATLAS_CMD_DEVICE_SHUTDOWN_CLOUD)
        commandTypeDevice_ = AtlasCommandDeviceType::ATLAS_CMD_DEVICE_SHUTDOWN;
    else
        commandTypeDevice_ = AtlasCommandDeviceType::ATLAS_CMD_DEVICE_UNKNOWN;
}

} // namespace atlas

