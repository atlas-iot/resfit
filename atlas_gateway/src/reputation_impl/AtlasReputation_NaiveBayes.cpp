#include "AtlasReputation_NaiveBayes.h"
#include "../logger/AtlasLogger.h"
#include <iostream>
#include <string>

namespace atlas
{

double AtlasReputationNaiveBayes::computeReputationForFeature(const AtlasDeviceFeatureManager& manager,
                                                              const AtlasDeviceFeature &feature)
{       
    if (manager.getTotalTransactions() == 0 || manager.getTotalSuccessfulTransactions() == 0 ||
        feature.getSuccessfulTransactions() == 0) {
        std::string tmpInfo = "AtlasReputationNaiveBayes_computeForFeature: Reputation could not be calculated for feature no. " +
                              std::to_string((int) feature.getFeatureType()) + " !";
        ATLAS_LOGGER_INFO(tmpInfo.c_str());
        return 0;
    }    
    
    double featureProb = (double) feature.getSuccessfulTransactions() / (double)manager.getTotalSuccessfulTransactions();
    
    std::string tmpInfo = "AtlasReputationNaiveBayes_computeForFeature: Reputation calculated successfully for feature no. " +
                          std::to_string((int) feature.getFeatureType()) + " !";
    
    ATLAS_LOGGER_INFO(tmpInfo.c_str());
    
    return featureProb;
}

double AtlasReputationNaiveBayes::computeReputation(AtlasDeviceFeatureManager& manager,
                                                    std::vector<std::pair<AtlasDeviceFeatureType, double>>& feedbackMatrix)
{
    double satisfactionScore = 0;
    
    manager.updateTotalTransactions();

    /* Compute feedback per device */
    for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++)
        satisfactionScore += (*it).second * manager[(*it).first].getWeight();

    /* Check if feedback per device is >= feedback threshold */
    if (satisfactionScore >= manager.getFeedbackThreshold()) {
        manager.updateTotalSuccessfulTransactions();

        /* Update success transactions for each feature, if the weighted feedback for each one is >= weighted threshold */
        double weightedFeatureFeadback = 0, weightedFeatureFeedbackThreshold = 0;
        for (auto it = feedbackMatrix.begin(); it != feedbackMatrix.end(); it++) {
            weightedFeatureFeadback = (*it).second * manager[(*it).first].getWeight();
            weightedFeatureFeedbackThreshold = manager.getFeedbackThreshold() * manager[(*it).first].getWeight();

            if (weightedFeatureFeadback >= weightedFeatureFeedbackThreshold) {
                manager[(*it).first].updateSuccessfulTransactions();
            }
        }
    }

    return computeReputation(manager);
}

double AtlasReputationNaiveBayes::computeReputation(AtlasDeviceFeatureManager& manager)
{
    if (manager.getTotalTransactions() == 0)
        return 0;

    /* Compute reputation for device */
    double repVal = (double)manager.getTotalSuccessfulTransactions() / (double)manager.getTotalTransactions(); 
    for (const auto &feature : manager.getDeviceFeatures())
        repVal *= computeReputationForFeature(manager, feature);

    manager.updateReputation(repVal);

    return repVal;
}

} //namespace atlas
