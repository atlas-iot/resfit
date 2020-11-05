#include <string>
#include <vector>
#include <boost/bind.hpp>
#include "AtlasTelemetry.h"
#include "../logger/AtlasLogger.h"
#include "../coap/AtlasCoapServer.h"
#include "../device/AtlasDeviceManager.h"
#include "../commands/AtlasCommandType.h"
#include "../identity/AtlasIdentity.h"

namespace atlas {

const std::string ATLAS_TELEMETRY_HOSTNAME_URI          = "gateway/telemetry/hostname";
const std::string ATLAS_TELEMETRY_KERN_INFO_URI         = "gateway/telemetry/kernel_info";
const std::string ATLAS_TELEMETRY_SYSINFO_UPTIME_URI    = "gateway/telemetry/sysinfo/uptime";
const std::string ATLAS_TELEMETRY_SYSINFO_TOTALRAM_URI  = "gateway/telemetry/sysinfo/totalram";
const std::string ATLAS_TELEMETRY_SYSINFO_FREERAM_URI   = "gateway/telemetry/sysinfo/freeram";
const std::string ATLAS_TELEMETRY_SYSINFO_SHAREDRAM_URI = "gateway/telemetry/sysinfo/sharedram";
const std::string ATLAS_TELEMETRY_SYSINFO_BUFFERRAM_URI = "gateway/telemetry/sysinfo/bufferram";
const std::string ATLAS_TELEMETRY_SYSINFO_TOTALSWAP_URI = "gateway/telemetry/sysinfo/totalswap";
const std::string ATLAS_TELEMETRY_SYSINFO_FREESWAP_URI  = "gateway/telemetry/sysinfo/freeswap";
const std::string ATLAS_TELEMETRY_SYSINFO_PROCS_URI     = "gateway/telemetry/sysinfo/procs";
const std::string ATLAS_TELEMETRY_SYSINFO_LOAD1_URI     = "gateway/telemetry/sysinfo/load1";
const std::string ATLAS_TELEMETRY_SYSINFO_LOAD5_URI     = "gateway/telemetry/sysinfo/load5";
const std::string ATLAS_TELEMETRY_SYSINFO_LOAD15_URI    = "gateway/telemetry/sysinfo/load15";
const std::string ATLAS_TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE_URI    = "gateway/telemetry/packets_per_minute";
const std::string ATLAS_TELEMETRY_PACKETS_INFO_PACKETS_AVG_URI    = "gateway/telemetry/packets_avg";


AtlasTelemetry::AtlasTelemetry() : hostnameResource_(ATLAS_TELEMETRY_HOSTNAME_URI,
                                                     ATLAS_COAP_METHOD_PUT,
                                                     boost::bind(&AtlasTelemetry::featureCallback,
                                                                 this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   kernInfoResource_(ATLAS_TELEMETRY_KERN_INFO_URI,
                                                     ATLAS_COAP_METHOD_PUT,
                                                     boost::bind(&AtlasTelemetry::featureCallback,
                                                                 this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   uptimeResource_(ATLAS_TELEMETRY_SYSINFO_UPTIME_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   totalramResource_(ATLAS_TELEMETRY_SYSINFO_TOTALRAM_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   freeramResource_(ATLAS_TELEMETRY_SYSINFO_FREERAM_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   sharedramResource_(ATLAS_TELEMETRY_SYSINFO_SHAREDRAM_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   bufferramResource_(ATLAS_TELEMETRY_SYSINFO_BUFFERRAM_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   totalswapResource_(ATLAS_TELEMETRY_SYSINFO_TOTALSWAP_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   freeswapResource_(ATLAS_TELEMETRY_SYSINFO_FREESWAP_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   procsResource_(ATLAS_TELEMETRY_SYSINFO_PROCS_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   load1Resource_(ATLAS_TELEMETRY_SYSINFO_LOAD1_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   load5Resource_(ATLAS_TELEMETRY_SYSINFO_LOAD5_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   load15Resource_(ATLAS_TELEMETRY_SYSINFO_LOAD15_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   packetsPerMinuteResource_(ATLAS_TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8)),
                                   packetsAvgResource_(ATLAS_TELEMETRY_PACKETS_INFO_PACKETS_AVG_URI,
                                                   ATLAS_COAP_METHOD_PUT,
                                                   boost::bind(&AtlasTelemetry::featureCallback,
                                                               this, _1, _2, _3, _4, _5, _6, _7, _8))
{
    ATLAS_LOGGER_DEBUG("Start telemetry module");
 
    /* Add CoAP resource for each telemetry feature */
    AtlasCoapServer::getInstance().addResource(hostnameResource_);
    AtlasCoapServer::getInstance().addResource(kernInfoResource_);
    AtlasCoapServer::getInstance().addResource(uptimeResource_);
    AtlasCoapServer::getInstance().addResource(totalramResource_);
    AtlasCoapServer::getInstance().addResource(freeramResource_);
    AtlasCoapServer::getInstance().addResource(sharedramResource_);
    AtlasCoapServer::getInstance().addResource(bufferramResource_);
    AtlasCoapServer::getInstance().addResource(totalswapResource_);
    AtlasCoapServer::getInstance().addResource(freeswapResource_);
    AtlasCoapServer::getInstance().addResource(procsResource_);
    AtlasCoapServer::getInstance().addResource(load1Resource_);
    AtlasCoapServer::getInstance().addResource(load5Resource_);
    AtlasCoapServer::getInstance().addResource(load15Resource_);
    AtlasCoapServer::getInstance().addResource(packetsPerMinuteResource_);
    AtlasCoapServer::getInstance().addResource(packetsAvgResource_);
}

std::pair<std::string,std::string> AtlasTelemetry::getFeature(const std::string &path, AtlasCommand &cmd)
{
    std::pair<std::string, std::string> ret;
    ret.first = "";
    ret.second = "";

    if (!cmd.getLen()) {
        ATLAS_LOGGER_ERROR("Telemetry end-point called with invalid feature length");
        return ret;
    }
    if (std::strstr(path.c_str(), ATLAS_TELEMETRY_HOSTNAME_URI.c_str()) &&
        cmd.getType() == ATLAS_CMD_TELEMETRY_HOSTNAME)
        ret.first = TELEMETRY_HOSTNAME;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_KERN_INFO_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_KERN_INFO)
        ret.first = TELEMETRY_KERN_INFO;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_UPTIME_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_UPTIME)
        ret.first = TELEMETRY_SYSINFO_UPTIME;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_TOTALRAM_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_TOTALRAM)
        ret.first = TELEMETRY_SYSINFO_TOTALRAM;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_FREERAM_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_FREERAM)
        ret.first = TELEMETRY_SYSINFO_FREERAM;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_SHAREDRAM_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_SHAREDRAM)
        ret.first = TELEMETRY_SYSINFO_SHAREDRAM;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_BUFFERRAM_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_BUFFERRAM)
        ret.first = TELEMETRY_SYSINFO_BUFFERRAM;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_TOTALSWAP_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_TOTALSWAP)
        ret.first = TELEMETRY_SYSINFO_TOTALSWAP;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_FREESWAP_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_FREESWAP)
        ret.first = TELEMETRY_SYSINFO_FREESWAP;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_PROCS_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_PROCS)
        ret.first = TELEMETRY_SYSINFO_PROCS;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_LOAD1_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_LOAD1)
        ret.first = TELEMETRY_SYSINFO_LOAD1;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_LOAD5_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_LOAD5)
        ret.first = TELEMETRY_SYSINFO_LOAD5;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_SYSINFO_LOAD15_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_SYSINFO_LOAD15)
        ret.first = TELEMETRY_SYSINFO_LOAD15;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_PACKETS_PER_MINUTE)
        ret.first = TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE;
    else if (std::strstr(path.c_str(), ATLAS_TELEMETRY_PACKETS_INFO_PACKETS_AVG_URI.c_str()) &&
             cmd.getType() == ATLAS_CMD_TELEMETRY_PACKETS_AVG)
        ret.first = TELEMETRY_PACKETS_INFO_PACKETS_AVG;
 
    ret.second.assign((char *)cmd.getVal(), cmd.getLen());

    return ret;
}

AtlasCoapResponse AtlasTelemetry::featureCallback(const std::string &path, const std::string &pskIdentity,
                                                  const std::string& psk, AtlasCoapMethod method,
                                                  const uint8_t* reqPayload, size_t reqPayloadLen,
                                                  uint8_t **respPayload, size_t *respPayloadLen)
{
    AtlasCommandBatch cmdBatch;
    std::vector<AtlasCommand> cmd;
    std::string identity = "";
    std::pair<std::string, std::string> feature;
    
    ATLAS_LOGGER_DEBUG("Telemetry hostname callback executed...");

    ATLAS_LOGGER_INFO1("Process TELEMETRY command from client with DTLS PSK identity ", pskIdentity);

    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(pskIdentity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + pskIdentity);
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    if (!device->isRegistered()) {
        ATLAS_LOGGER_ERROR("Received TELEMETRY command for a device which is not registered...");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Parse commands */
    cmdBatch.setRawCommands(reqPayload, reqPayloadLen);
    cmd = cmdBatch.getParsedCommands();

    if (cmd.empty()) {
        ATLAS_LOGGER_ERROR("Telemetry end-point called with empty command set");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    for (AtlasCommand &cmdEntry : cmd) {
        if (cmdEntry.getType() == ATLAS_CMD_IDENTITY) {
            if (!cmdEntry.getLen()) {
                ATLAS_LOGGER_ERROR("Telemetry end-point called with empty IDENTITY command");
                printf("Am primit\n");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
            
            identity.assign((char *)cmdEntry.getVal(), cmdEntry.getLen());
            if (pskIdentity != identity) {
                ATLAS_LOGGER_ERROR("Telemetry hostname end-point called with SPOOFED identity");
                return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
            }
        } else
            feature = getFeature(path, cmdEntry);
    }

    if (identity == "") {
        ATLAS_LOGGER_ERROR("Telemetry failed because of invalid identity");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }
    if (feature.first == "" || feature.second == "") {
        ATLAS_LOGGER_ERROR("Telemetry failed because of invalid feature");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE;
    }

    /* Set device telemetry feature */
    device->setFeature(feature.first, feature.second);

    ATLAS_LOGGER_INFO1("Device telemetry state for identity " + identity + " changed to ", device->telemetryInfoToJSON());
    
    return ATLAS_COAP_RESP_OK;
}

} // namespace atlas
