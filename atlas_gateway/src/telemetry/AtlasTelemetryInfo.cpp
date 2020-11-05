#include "AtlasTelemetryInfo.h"

namespace atlas {

namespace {

const std::string ATLAS_TELEMETRY_DEFAULT_VALUE = "N/A";

} //anonymous namespace

AtlasTelemetryInfo::AtlasTelemetryInfo()
{
    /* Add all features with a default value */
    setFeature(TELEMETRY_HOSTNAME, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_KERN_INFO, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_UPTIME, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_TOTALRAM, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_FREERAM, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_SHAREDRAM, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_BUFFERRAM, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_TOTALSWAP, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_FREESWAP, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_PROCS, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_LOAD1, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_LOAD5, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_SYSINFO_LOAD15, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE, ATLAS_TELEMETRY_DEFAULT_VALUE);
    setFeature(TELEMETRY_PACKETS_INFO_PACKETS_AVG, ATLAS_TELEMETRY_DEFAULT_VALUE);
}

void AtlasTelemetryInfo::clearFeatures()
{
    for (auto it = features_.begin(); it != features_.end(); ++it)
        (*it).second = ATLAS_TELEMETRY_DEFAULT_VALUE;
}

std::string AtlasTelemetryInfo::toJSON(const std::string &feature)
{
    std::string featureString;

    /* Dump all features */
    if (feature == "") {
        auto it = features_.begin();
        while (it != features_.end()) {
            featureString += "\"" + (*it).first + "\": \"" + (*it).second + "\"";
            ++it;
            if (it != features_.end())
                featureString += ",\n";
        }
    } else
        featureString += "\"" + feature +"\": \"" + features_[feature] + "\"";

    return featureString;
}

} // namespace atlas
