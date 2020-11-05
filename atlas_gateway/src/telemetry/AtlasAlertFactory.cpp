#include "AtlasAlertFactory.h"
#include "AtlasTelemetryInfo.h"

namespace atlas {

namespace {

/* Push alerts default values */

/* Sysinfo procs default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT = 300;
const std::string ATLAS_PUSH_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/push";

/* Sysinfo uptime default push alert value */
const uint16_t ATLAS_PUSH_ALERT_UPTIME_RATE_SEC_DEFAULT = 120;
const std::string ATLAS_PUSH_ALERT_UPTIME_PATH = "client/telemetry/sysinfo/uptime/alerts/push";

/* Sysinfo freeram default push alert values */
const uint16_t ATLAS_PUSH_ALERT_FREERAM_RATE_SEC_DEFAULT = 300;
const std::string ATLAS_PUSH_ALERT_FREERAM_PATH = "client/telemetry/sysinfo/freeram/alerts/push";

/* Sysinfo sharedram default push alert values */
const uint16_t ATLAS_PUSH_ALERT_SHAREDRAM_RATE_SEC_DEFAULT = 300;
const std::string ATLAS_PUSH_ALERT_SHAREDRAM_PATH = "client/telemetry/sysinfo/sharedram/alerts/push";

/* Sysinfo bufferram default push alert values */
const uint16_t ATLAS_PUSH_ALERT_BUFFERRAM_RATE_SEC_DEFAULT = 300;
const std::string ATLAS_PUSH_ALERT_BUFFERRAM_PATH = "client/telemetry/sysinfo/bufferram/alerts/push";

/* Sysinfo totalswap default push alert values */
const uint16_t ATLAS_PUSH_ALERT_TOTALSWAP_RATE_SEC_DEFAULT = 300;
const std::string ATLAS_PUSH_ALERT_TOTALSWAP_PATH = "client/telemetry/sysinfo/totalswap/alerts/push";

/* Sysinfo freeswap default push alert values */
const uint16_t ATLAS_PUSH_ALERT_FREESWAP_RATE_SEC_DEFAULT = 300;
const std::string ATLAS_PUSH_ALERT_FREESWAP_PATH = "client/telemetry/sysinfo/freeswap/alerts/push";

/* Sysinfo load1 default push alert values */
const uint16_t ATLAS_PUSH_ALERT_LOAD1_RATE_SEC_DEFAULT = 120;
const std::string ATLAS_PUSH_ALERT_LOAD1_PATH = "client/telemetry/sysinfo/load1/alerts/push";

/* Sysinfo load5 default push alert values */
const uint16_t ATLAS_PUSH_ALERT_LOAD5_RATE_SEC_DEFAULT = 120;
const std::string ATLAS_PUSH_ALERT_LOAD5_PATH = "client/telemetry/sysinfo/load5/alerts/push";

/* Sysinfo load15 default push alert values */
const uint16_t ATLAS_PUSH_ALERT_LOAD15_RATE_SEC_DEFAULT = 120;
const std::string ATLAS_PUSH_ALERT_LOAD15_PATH = "client/telemetry/sysinfo/load15/alerts/push";

/* Packets per minute default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PACKETS_PER_MINUTE_RATE_SEC_DEFAULT = 60;
const std::string ATLAS_PUSH_ALERT_PACKETS_PER_MIN_PATH = "client/telemetry/packets_info/packets_per_min/alerts/push";

/* Packets average length default push alert values */
const uint16_t ATLAS_PUSH_ALERT_PACKETS_AVG_RATE_SEC_DEFAULT = 60;
const std::string ATLAS_PUSH_ALERT_PACKETS_AVG_PATH = "client/telemetry/packets_info/packets_avg/alerts/push";

/* Threshold alert default values */

/* Sysinfo procs default threshold alert value */
const uint16_t ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT = 10;
const std::string ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT = "1000";
const std::string ATLAS_THRESHOLD_ALERT_PROCS_PATH = "client/telemetry/sysinfo/procs/alerts/threshold";

} // namespace anonymous

AtlasPushAlert* AtlasAlertFactory::getPushAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PROCS_PATH,
                                  ATLAS_PUSH_ALERT_PROCS_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_UPTIME)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_UPTIME_PATH,
                                  ATLAS_PUSH_ALERT_UPTIME_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_FREERAM)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_FREERAM_PATH,
                                  ATLAS_PUSH_ALERT_FREERAM_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_SHAREDRAM)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_SHAREDRAM_PATH,
                                  ATLAS_PUSH_ALERT_SHAREDRAM_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_BUFFERRAM)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_BUFFERRAM_PATH,
                                  ATLAS_PUSH_ALERT_BUFFERRAM_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_TOTALSWAP)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_TOTALSWAP_PATH,
                                  ATLAS_PUSH_ALERT_TOTALSWAP_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_FREESWAP)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_FREESWAP_PATH,
                                  ATLAS_PUSH_ALERT_FREESWAP_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_LOAD1)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_LOAD1_PATH,
                                  ATLAS_PUSH_ALERT_LOAD1_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_LOAD5)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_LOAD5_PATH,
                                  ATLAS_PUSH_ALERT_LOAD5_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_SYSINFO_LOAD15)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_LOAD15_PATH,
                                  ATLAS_PUSH_ALERT_LOAD15_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PACKETS_PER_MIN_PATH,
                                  ATLAS_PUSH_ALERT_PACKETS_PER_MINUTE_RATE_SEC_DEFAULT);
    else if (type == TELEMETRY_PACKETS_INFO_PACKETS_AVG)
        return new AtlasPushAlert(deviceIdentity, ATLAS_PUSH_ALERT_PACKETS_AVG_PATH,
                                  ATLAS_PUSH_ALERT_PACKETS_AVG_RATE_SEC_DEFAULT);
    
    return nullptr;
}

AtlasThresholdAlert* AtlasAlertFactory::getThresholdAlert(const std::string& type, const std::string &deviceIdentity)
{
    if (type == TELEMETRY_SYSINFO_PROCS)
        return new AtlasThresholdAlert(deviceIdentity, ATLAS_THRESHOLD_ALERT_PROCS_PATH,
                                       ATLAS_THRESHOLD_ALERT_PROCS_RATE_SEC_DEFAULT,
                                       ATLAS_THRESHOLD_ALERT_PROCS_THRESHOLD_DEFAULT);
    
    return nullptr;
}

} // namespace atlas
