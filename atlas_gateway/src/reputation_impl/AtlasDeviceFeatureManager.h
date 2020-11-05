#ifndef __ATLAS_DEVICE_FEATURE_MANAGERR_H__
#define __ATLAS_DEVICE_FEATURE_MANAGERR_H__

#define ATLAS_DEVICE_FEATURE_MANAGER_DEFAULT_THRESHOLD 0.8
#define ATLAS_DEVICE_FEATURE_MANAGER_DATAPLANE_WEIGHT 0.6
#define ATLAS_DEVICE_FEATURE_MANAGER_CONTROLPLANE_WEIGHT 0.4

#include "AtlasDeviceFeatureType.h"
#include "AtlasDeviceFeature.h"

namespace atlas
{
class AtlasDeviceFeatureManager
{
public:
    /**
    * @brief Ctor for feature manager
    * @return none
    */
    AtlasDeviceFeatureManager() : totalSuccessTrans_(0), totalTrans_(0),
                                  deviceReputation_(0),
                                  feedbackThreshold_(ATLAS_DEVICE_FEATURE_MANAGER_DEFAULT_THRESHOLD) {};
    
    /**
     * @brief Add a new feature to a device or update the value of an existing feature
     * @param[in] Feature type
     * @param[in] Feature weight value
     * @return True is feature is new or False is feature existed (weight value will be updated)
    */
    bool addFeature(AtlasDeviceFeatureType type, double featureWeight);

    /**
     * @brief Remove a feature from a device
     * @param[in] Feature to be deleted identified by its type
     * @return True is the deletion succeedes. False otherwise.
    */
    bool removeFeature(AtlasDeviceFeatureType type);

    /**
     * @brief Updates number of successful transactions (required by Naive Bayes component)
     * @return none
    */
    void updateTotalSuccessfulTransactions() { ++totalSuccessTrans_; }
        
    /**
     * @brief Return number of successful transactions (required by Naive Bayes component)
     * @return Number of Successful Transactions
    */
    int getTotalSuccessfulTransactions() const { return totalSuccessTrans_; }

    /**
     * @brief Set number of successful transactions (required by Naive Bayes component)
     * @param[in] Number of successful transactions
     * @return none
    */
    void setTotalSuccessfulTransactions(int totalSuccessTrans) { totalSuccessTrans_ = totalSuccessTrans; }

    /**
     * @brief Update total number of interactions (required by Naive Bayes component)
     * @return none
    */
    void updateTotalTransactions() { ++totalTrans_; }

    /**
     * @brief Returns the total number of transactions (required by Naive Bayes component)
     * @return Number of transactions
    */
    int getTotalTransactions() const { return totalTrans_; }

    /**
     * @brief Set the total number of transactions (required by Naive Bayes component)
     * @param[in] Total number of transactions
     * @return none
    */
    void setTotalTransactions(int totalTrans) { totalTrans_ = totalTrans; }

    /**
     * @brief Updates the device reputation value
     * @param[in] New device reputation value
     * @return none
    */
    void updateReputation(double newVal) { deviceReputation_ = newVal; }

    /**
     * @brief Returns the reputation value of current device
     * @return Reputation value
    */
    double getReputationScore() const { return deviceReputation_; }

    /**
     * @brief Updates the device reputation value
     * @param[in] New device reputation value
     * @return none
    */
    void updateFeedbackThreshold(double newVal) { feedbackThreshold_ = newVal; }

    /**
     * @brief Returns the reputation value of current device
     * @return Reputation value
    */
    double getFeedbackThreshold() const { return feedbackThreshold_; }

    /**
     * @brief Returns all features that the current device has
     * @return Vector with all device features
    */
    std::vector<AtlasDeviceFeature> getDeviceFeatures() { return features_; }

    AtlasDeviceFeature& operator [] (AtlasDeviceFeatureType type);

private:
    std::vector<AtlasDeviceFeature> features_;
    int totalSuccessTrans_;
    int totalTrans_;
    double deviceReputation_;
    double feedbackThreshold_;
};
} //namespace atlas

#endif /*__ATLAS_DEVICE_FEATURE_MANAGER_H__*/
