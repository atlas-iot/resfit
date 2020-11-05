#include <iostream>
#include <string>
#include <vector>
#include "AtlasReputation_Tester.h"

namespace atlas 
{
double AtlasReputationTester::generateFeedBack(double badFeedbackProb, double feedbackThreshold)
{
    double tmpRand = rand() % 100;
    double rez = 0;
    if (tmpRand > badFeedbackProb) {
        rez = rand() % (int)(101 - feedbackThreshold * 100);
        rez = rez + (feedbackThreshold * 100);
    } else {
        rez = rand() % (int)(1 + feedbackThreshold * 100);
    }

    return (rez / 100);
}

void AtlasReputationTester::simulateScenario_1(int noOfClients, int noOfFeatures, double badFeedbackProb, double thresholdVal, int noOfIterations)
{ 
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_1_client_" + std::to_string(i) + ".dat";
        AtlasDeviceFeatureManager manager;
        manager.updateFeedbackThreshold(thresholdVal);

        for (int j=0; j < noOfFeatures; j++) {
            manager.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {            
                for (size_t k=0; k < manager.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_2(int noOfClients, int noOfFeatures, double badFeedbackProb, double thresholdVal, int noOfIterations)
{
    double localBadFeedbackProb = badFeedbackProb;
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_2_client_" + std::to_string(i) + ".dat";
        AtlasDeviceFeatureManager manager;
        manager.updateFeedbackThreshold(thresholdVal);

        for (int j=0; j < noOfFeatures; j++) {
            manager.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;

            for (int j=0; j < noOfIterations; j++) {  
                if ((i > (noOfClients / 2)) && (j > (noOfIterations / 2))) {
                    localBadFeedbackProb = 90;
                }         
                for (int k=0; k < noOfFeatures; k++) {
                    genFB = generateFeedBack(localBadFeedbackProb, thresholdVal);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_3(int noOfClients, int noOfFeatures, double badFeedbackProb, double thresholdVal, int noOfIterations)
{
    double localBadFeedbackProb;
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_3_client_" + std::to_string(i) + ".dat";
        AtlasDeviceFeatureManager manager;
        manager.updateFeedbackThreshold(thresholdVal);

        for (int j=0; j < noOfFeatures; j++) {
            manager.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            localBadFeedbackProb = badFeedbackProb;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;   

            //First half of the scenario, with normal feedback
            for (int j = 0; j < (noOfIterations/2); j++) {            
                for (int k=0; k < noOfFeatures; k++) {
                    genFB = generateFeedBack(localBadFeedbackProb, thresholdVal);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }

            //Second half of the scenario with worse feedback (error is 50% greater)
            localBadFeedbackProb = 90;
            for (int j = 0; j < (noOfIterations/2); j++) {            
                for (int k=0; k < noOfFeatures; k++) {
                    genFB = generateFeedBack(localBadFeedbackProb, thresholdVal);
                    tmpPair.first = manager.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }

                rez = AtlasReputationNaiveBayes::computeReputation(manager, fbMatrix);
                fprintf(fileOut, "%f\n", rez);
                fbMatrix.clear();
            }

            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_4(int noOfClients, int noOfFeatures, double badFeedbackProb, double thresholdVal, int noOfIterations)
{
    double weightOfControlPlane = 0.35;    
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_4_client_" + std::to_string(i) + ".dat";
        
        /***** Manager for Data Plane features *****/
        AtlasDeviceFeatureManager managerData;
        managerData.updateFeedbackThreshold(thresholdVal);
        for (int j=0; j < noOfFeatures; j++) {
            managerData.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        /***** Manager for Control Plane features *****/
        AtlasDeviceFeatureManager managerCtrl;
        managerCtrl.updateFeedbackThreshold(thresholdVal);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME, 0.3);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS, 0.3);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS, 0.4);

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rezData = 0, rezCtrl = 0, rez, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {
                /***** Generate feedback and compute reputation for Data Plane *****/            
                for (size_t k=0; k < managerData.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = managerData.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rezData = AtlasReputationNaiveBayes::computeReputation(managerData, fbMatrix);
                fbMatrix.clear();

                /***** Generate feedback and compute reputation for Control Plane *****/            
                for (size_t k=0; k < managerCtrl.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = managerCtrl.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rezCtrl = AtlasReputationNaiveBayes::computeReputation(managerCtrl, fbMatrix);
                fbMatrix.clear();

                /***** Generate feedback for device *****/
                rez = (rezCtrl * weightOfControlPlane) + (rezData * (1 - weightOfControlPlane));
                fprintf(fileOut, "%f\n", rez);
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_5(int noOfClients, double badFeedbackProb, double thresholdVal, int noOfIterations)
{
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_5_client_" + std::to_string(i) + ".dat";
        
        /***** Manager for Control Plane features *****/
        AtlasDeviceFeatureManager managerCtrl;
        managerCtrl.updateFeedbackThreshold(thresholdVal);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME, 0.3);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS, 0.3);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS, 0.4);

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {         
                for (size_t k=0; k < managerCtrl.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = managerCtrl.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rez = AtlasReputationNaiveBayes::computeReputation(managerCtrl, fbMatrix);
                fbMatrix.clear();
                fprintf(fileOut, "%f\n", rez);
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_6(int noOfClients, double regTime, double kaPkts, double validPkts, double badFeedbackProb, double thresholdVal, int noOfIterations)
{
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_6_client_" + std::to_string(i) + ".dat";
        
        /***** Manager for Control Plane features *****/
        AtlasDeviceFeatureManager managerCtrl;
        managerCtrl.updateFeedbackThreshold(thresholdVal);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME, regTime);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS, kaPkts);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS, validPkts);

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rez = 0, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {         
                for (size_t k=0; k < managerCtrl.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = managerCtrl.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rez = AtlasReputationNaiveBayes::computeReputation(managerCtrl, fbMatrix);
                fbMatrix.clear();
                fprintf(fileOut, "%f\n", rez);
            }
            fclose(fileOut);
        }
    }
}

void AtlasReputationTester::simulateScenario_7(int noOfClients, int noOfFeatures, double ctrlPlaneWeight, double regTime, double kaPkts, double validPkts, double badFeedbackProb, double thresholdVal, int noOfIterations)
{
    double weightOfControlPlane = ctrlPlaneWeight;    
    for (int i=1; i <= noOfClients; i++) {
        std::string fileName = "scenario_7_client_" + std::to_string(i) + ".dat";
        
        /***** Manager for Data Plane features *****/
        AtlasDeviceFeatureManager managerData;
        managerData.updateFeedbackThreshold(thresholdVal);
        for (int j=0; j < noOfFeatures; j++) {
            managerData.addFeature((AtlasDeviceFeatureType)j, 1.0/(double)noOfFeatures);
        }

        /***** Manager for Control Plane features *****/
        AtlasDeviceFeatureManager managerCtrl;
        managerCtrl.updateFeedbackThreshold(thresholdVal);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME, regTime);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS, kaPkts);
        managerCtrl.addFeature(AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS, validPkts);

        FILE* fileOut = fopen(fileName.c_str(), "w");
        if (fileOut != nullptr) {
            double rezData = 0, rezCtrl = 0, rez, genFB = 0;
            std::vector<std::pair<AtlasDeviceFeatureType, double>> fbMatrix;
            std::pair<AtlasDeviceFeatureType, double> tmpPair;
            
            for (int j = 0; j < noOfIterations; j++) {
                /***** Generate feedback and compute reputation for Data Plane *****/            
                for (size_t k=0; k < managerData.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = managerData.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rezData = AtlasReputationNaiveBayes::computeReputation(managerData, fbMatrix);
                fbMatrix.clear();

                /***** Generate feedback and compute reputation for Control Plane *****/            
                for (size_t k=0; k < managerCtrl.getDeviceFeatures().size(); k++) {
                    genFB = generateFeedBack(badFeedbackProb, thresholdVal);
                    tmpPair.first = managerCtrl.getDeviceFeatures()[k].getFeatureType();
                    tmpPair.second = genFB;
                    fbMatrix.push_back(tmpPair);
                }
                rezCtrl = AtlasReputationNaiveBayes::computeReputation(managerCtrl, fbMatrix);
                fbMatrix.clear();

                /***** Generate feedback for device *****/
                rez = (rezCtrl * weightOfControlPlane) + (rezData * (1 - weightOfControlPlane));
                fprintf(fileOut, "%f\n", rez);
            }
            fclose(fileOut);
        }
    }
}
} //namespace atlas
