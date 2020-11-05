#ifndef __ATLAS_REPUTATION_TESTER_H__
#define __ATLAS_REPUTATION_TESTER_H__

#include "../AtlasReputation_NaiveBayes.h"

namespace atlas
{
class AtlasReputationTester
{
public:
    static double generateFeedBack(double badFeedbackProb, double feedbackThreshold);
        
    /**
     * @brief Simulation containing: 'c' clients with 'f' features, no attacks
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_1(int, int, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with 'f' features, wrong data attack
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_2(int, int, double, double, int);    

    /**
     * @brief Simulation containing: 'c' clients with 'f' features, change of feedback at half of simulation
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_3(int, int, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with 'f' Data plane features and 3 Control plane features, no attacks 
     * @param[in] Number of clients
     * @param[in] Number of features for clients
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_4(int, int, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with all Control plane features having fixed weights (0.3, 0.3, 0.4), no attacks 
     * @param[in] Number of clients
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_5(int, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with all Control plane features having variabile weights, no attacks 
     * @param[in] Number of clients
     * @param[in] Weight of ATLAS_FEATURE_REGISTER_TIME feature
     * @param[in] Weight of ATLAS_FEATURE_KEEPALIVE_PACKETS feature
     * @param[in] Weight of ATLAS_FEATURE_VALID_PACKETS feature
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_6(int, double, double, double, double, double, int);

    /**
     * @brief Simulation containing: 'c' clients with 'f' Data plane features and all Control plane features (with variable weights), no attacks 
     * @param[in] Number of clients
     * @param[in] Number of Data plane features
     * @param[in] System features weight in device reputation computation
     * @param[in] Weight of ATLAS_FEATURE_REGISTER_TIME feature
     * @param[in] Weight of ATLAS_FEATURE_KEEPALIVE_PACKETS feature
     * @param[in] Weight of ATLAS_FEATURE_VALID_PACKETS feature
     * @param[in] Probability of bad feedback
     * @param[in] Threshold value for feedback
     * @param[in] Number of iterations for the simulation
     * @return Results are written in separate files depending on number of clients
    */
    static void simulateScenario_7(int, int, double, double, double, double, double, double, int);
};

} //namespace atlas

#endif /*__ATLAS_REPUTATION_TESTER_H__*/