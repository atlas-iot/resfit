#ifndef __ATLAS_DEVICE_FEATURE_H__
#define __ATLAS_DEVICE_FEATURE_H__

#include <stdio.h>
#include <vector>
#include "AtlasDeviceFeatureType.h"

namespace atlas
{
class AtlasDeviceFeature {
public:
    /**
     * @brief Default Ctor with FeatureType = GENERIC and Value = 0
     * @return none
    */
    AtlasDeviceFeature() : featureType_(AtlasDeviceFeatureType::ATLAS_DEVICE_FEATURE_GENERIC), weight_(1), successTrans_(0) {}

    /**
     * @brief Explicit Ctor for FeatureType
     * @param[in] Specific feature from a list of FeatureTypes
     * @param[in] Weight of feature in device reputation computation
     * @return none
    */
    AtlasDeviceFeature(AtlasDeviceFeatureType featureType, double featureWeight) : featureType_(featureType), weight_(featureWeight), successTrans_(0) {}

    /**
     * @brief Get feature type (const method)
     * @return Current feature type
    */
    AtlasDeviceFeatureType getFeatureType() const { return featureType_; }

    /**
     * @brief Get feature Weight value
     * @return Current Weight value
    */
    double getWeight() const { return weight_; }

    /**
     * @brief Set feature Weight value
     * @param[in] New Weight value
     * @return none
    */
    void updateWeight(double newVal) { weight_ = newVal; }

    /**
     * @brief Updates number of successful transactions (required by Naive Bayes component)
     * @return none
    */
    void updateSuccessfulTransactions() { ++successTrans_; }
        
    /**
     * @brief Return number of successful transactions (required by Naive Bayes component)
     * @return Number of Successful Transactions
    */
    int getSuccessfulTransactions() const { return successTrans_; }

    /**
     * @brief Set number of successful transactions (required by Naive Bayes component)
     * @param[in] Number of Successful Transactions
     * @return none
    */
    void setSuccessfulTransactions(int successTrans) { successTrans_ = successTrans; }

    AtlasDeviceFeature(const AtlasDeviceFeature &src) { featureType_ = src.featureType_;
                                                        successTrans_ = src.successTrans_;
                                                        weight_ = src.weight_; }
                                    
    AtlasDeviceFeature& operator = (const AtlasDeviceFeature& src) { featureType_ = src.featureType_;
                                                                     successTrans_ = src.successTrans_;
                                                                     weight_ = src.weight_; return *this; }

private:
    AtlasDeviceFeatureType featureType_;
    double weight_;
    int successTrans_;
};
} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_H__*/
