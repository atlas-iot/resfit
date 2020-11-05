#ifndef __ATLAS_DEVICE_FEATURE_TYPE_H__
#define __ATLAS_DEVICE_FEATURE_TYPE_H__

namespace atlas
{

enum class AtlasDeviceFeatureType {
    /* Features for data reputation network */
    ATLAS_DEVICE_FEATURE_SENSOR = 0,
    ATLAS_DEVICE_FEATURE_RESP_TIME,

    /* Features for system (control plane) reputation network */
    ATLAS_FEATURE_VALID_PACKETS = 1000,
    ATLAS_FEATURE_REGISTER_TIME,
    ATLAS_FEATURE_KEEPALIVE_PACKETS,

    /* Features for reputation network simulation */
    ATLAS_DEVICE_FEATURE_GENERIC = 2000,
    ATLAS_DEVICE_FEATURE_CO2,
    ATLAS_DEVICE_FEATURE_TEMPERATURE,
    ATLAS_DEVICE_FEATURE_LIGHT,
    ATLAS_DEVICE_FEATURE_HUMIDITY,
    ATLAS_DEVICE_FEATURE_AIR_PRESSURE,
 
};

enum class AtlasDeviceNetworkType {
    /* Identifies the system reputation network */
    ATLAS_NETWORK_SYSTEM = 0,

    /* Identifies the data reputation network */
    ATLAS_NETWORK_SENSOR_START = 100,
    ATLAS_NETWORK_SENSOR_TEMPERATURE = 101,
    ATLAS_NETWORK_SENSOR_MAX = 102,
};

} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_TYPE_H__*/
