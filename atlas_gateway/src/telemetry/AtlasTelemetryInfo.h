#ifndef __ATLAS_TELEMETRY_INFO_H__
#define __ATLAS_TELEMETRY_INFO_H__

#include <unordered_map>

namespace atlas {

const std::string TELEMETRY_HOSTNAME                        = "hostname";
const std::string TELEMETRY_KERN_INFO                       = "kernelInfo";
const std::string TELEMETRY_SYSINFO_UPTIME                  = "sysinfoUptime";
const std::string TELEMETRY_SYSINFO_TOTALRAM                = "sysinfoTotalram";
const std::string TELEMETRY_SYSINFO_FREERAM                 = "sysinfoFreeram";
const std::string TELEMETRY_SYSINFO_SHAREDRAM               = "sysinfoSharedram";
const std::string TELEMETRY_SYSINFO_BUFFERRAM               = "sysinfoBufferram";
const std::string TELEMETRY_SYSINFO_TOTALSWAP               = "sysinfoTotalswap";
const std::string TELEMETRY_SYSINFO_FREESWAP                = "sysinfoFreeswap";
const std::string TELEMETRY_SYSINFO_PROCS                   = "sysinfoProcs";
const std::string TELEMETRY_SYSINFO_LOAD1                   = "sysinfoLoad1";
const std::string TELEMETRY_SYSINFO_LOAD5                   = "sysinfoLoad5";
const std::string TELEMETRY_SYSINFO_LOAD15                  = "sysinfoLoad15";
const std::string TELEMETRY_PACKETS_INFO_PACKETS_PER_MINUTE = "packetsPerMinute";
const std::string TELEMETRY_PACKETS_INFO_PACKETS_AVG        = "packetsAvgLength";

class AtlasTelemetryInfo
{

public:
    /**
    * @brief Ctor for telemetry info
    * @return none
    */
    AtlasTelemetryInfo();

    /**
    * @brief Set telemetry feature
    * @param[in] feature Telemetry feature name
    * @param[in] value Telemetry feature value
    * @return none
    */
    inline void setFeature(const std::string &feature, const std::string &value) { features_[feature] = value; }
    
    /**
    * @brief Get telemetry feature
    * @param[in] feature Telemetry feature name
    * @return Telemetry feature value
    */
    inline std::string getFeature(const std::string &feature) { return features_[feature]; }

    /**
    * @brief Clear telemetry features
    * @return none
    */
    void clearFeatures();
   
    /**
    * @brief Convert telemetry info to JSON string
    * @param[in] feature Convert to JSON only the given feature. If this is empty all the features will be serialized
    * @return String containing all the key-value telemetry feature pairs
    */ 
    std::string toJSON(const std::string &feature = "");

private:
    /* Telemetry features */
    std::unordered_map<std::string, std::string> features_;
};

} // namespace atlas

#endif /* __ATLAS_TELEMETRY_INFO_H__ */
