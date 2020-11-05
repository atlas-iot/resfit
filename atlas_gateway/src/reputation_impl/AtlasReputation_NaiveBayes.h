#ifndef __ATLAS_REPUTATION_NAIVE_BAYES_H__
#define __ATLAS_REPUTATION_NAIVE_BAYES_H__

#define ATLAS_REPUTATION_NAIVE_BAYES_DECREASE_FACTOR 0.95

#include <unordered_map>
#include "AtlasDeviceFeatureManager.h"

namespace atlas
{
class AtlasReputationNaiveBayes
{
private:

    /**
     * @brief Computes reputation for a single Feature, either a DataPlane or ControlPlane type
     * @param[in] manager Feature manager
     * @param[in] feature Feature type
     * @return Reputation value of the feature
    */
    static double computeReputationForFeature(const AtlasDeviceFeatureManager &manager, const AtlasDeviceFeature &feature);

public:

    /**
     * @brief Generic computation of reputation for a Device, (Method used in simulations)
     * @param[in] manager Feature manager
     * @param[in] feedbackMatrix Vector of pairs containing FeatureType, as first value, and DataPlane Feedback, as second value
     * @return Reputation value for the Device
    */
    static double computeReputation(AtlasDeviceFeatureManager &manager,
                                    std::vector<std::pair<AtlasDeviceFeatureType, double>>& feedbackMatrix);

    /**
     * @brief Generic computation of reputation for a Device using the current reputation
     * statistics (without taking into consideration a feedback matrix)
     * @param[in] Feature manager
     * @return Reputation value for the Device
    */
    static double computeReputation(AtlasDeviceFeatureManager&);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_NAIVE_BAYES_H__*/
