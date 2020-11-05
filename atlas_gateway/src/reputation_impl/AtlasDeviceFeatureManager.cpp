#include "AtlasDeviceFeatureManager.h"
#include "../logger/AtlasLogger.h"

namespace atlas
{

bool AtlasDeviceFeatureManager::addFeature(AtlasDeviceFeatureType type, double featureWeight)
{
    for (auto it = features_.begin(); it != features_.end(); it++) {
        if ((*it).getFeatureType() == type) {
            ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: Feature already added. Updating value");
            (*it).updateWeight(featureWeight);
            return false;                       
        }        
    }

    features_.push_back(AtlasDeviceFeature(type, featureWeight));
    ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: New feature added to device");
    
    return true;
}

bool AtlasDeviceFeatureManager::removeFeature(AtlasDeviceFeatureType type)
{      
    int index = -1;
    for (auto it = features_.cbegin(); (it != features_.cend()) && ((*it).getFeatureType() != type); it++) {
        index++;
    }

    if (index != -1) {
        features_.erase(features_.cbegin() + index);
        ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: Feature deleted successfully");         
    } else {        
        ATLAS_LOGGER_INFO("AtlasDeviceFeatureManager: No features available for device. Delete operation aborted!");
        return false;
    }
    
    return true;
}

AtlasDeviceFeature& AtlasDeviceFeatureManager::operator [] (AtlasDeviceFeatureType type) 
{ 
    int pos = 0;

    for (auto it = features_.begin(); (it != features_.end()) && ((*it).getFeatureType() != type); it++) {
        pos++;
    }

    return features_[pos];
}

} //namespace atlas
