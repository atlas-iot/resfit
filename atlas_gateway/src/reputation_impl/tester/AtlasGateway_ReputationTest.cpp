#include <iostream>
#include <boost/program_options.hpp>
#include "../../logger/AtlasLogger.h"
#include "AtlasReputation_Tester.h"

namespace {

const std::string ATLAS_GATEWAY_DESC = "Atlas gateway - reputation tester";

/* Scenario number */
int scenarioNo;

/* Number of clients to be simulated */
int noOfClients;

/* Number of features for each client */
int noOfFeatures;

/* Reference value for generating data */
double badFeedbackProbab;

/* Threshold value for feedback */
double thresholdValue;

/* Number of iterations for simulation */
int noOfIterations;

/* ATLAS_FEATURE_REGISTER_TIME weight */
double regTimeFeature;

/* ATLAS_FEATURE_KEEPALIVE_PACKETS weight */
double kaPktsFeature;

/* ATLAS_FEATURE_VALID_PACKETS weight */
double valPktsFeature;

/* Control plane weigth in device reputation calculation */
double sysFeatWeight;

} // anonymous namespace

void parse_options(int argc, char **argv)
{
    boost::program_options::options_description desc(ATLAS_GATEWAY_DESC);
    boost::program_options::variables_map vm;
    
    desc.add_options()
    ("help", "Display help message")
    ("scenario_number,n", boost::program_options::value<int>(&scenarioNo), "Scenario number: between 1 to 7. See README for details regarding each scenario.")
    ("no_of_clients,c", boost::program_options::value<int>(&noOfClients), "Number of clients that will be simulated (between 1 and 10)")
    ("no_of_features,f", boost::program_options::value<int>(&noOfFeatures), "Number of dataplane features simulated for each client (between 1 and 5)")
    ("bad_fb_probability,p", boost::program_options::value<double>(&badFeedbackProbab), "Bad feedback probability when generating random feedback values for dataplane (between 1 and 99)")
    ("threshold_value,t", boost::program_options::value<double>(&thresholdValue), "Feedback threshold value (between 0.1 and 0.95)")
    ("no_of_iterations,i", boost::program_options::value<int>(&noOfIterations), "Number of iterations the simulation should be run (between 1 and 50000)")
    ("sys_feat_weight,s", boost::program_options::value<double>(&sysFeatWeight), "Weight of all System features in the computation of device reputation (between 0.01 and 0.99)")
    ("reg_time_weight,r", boost::program_options::value<double>(&regTimeFeature), "Weight of ATLAS_FEATURE_REGISTER_TIME feature (between 0.01 and 0.99). Sum of all three system features must be equal to 1.")
    ("ka_pkts_weight,k", boost::program_options::value<double>(&kaPktsFeature), "Weight of ATLAS_FEATURE_KEEPALIVE_PACKETS feature (between 0.01 and 0.99). Sum of all three system features must be equal to 1.")
    ("valid_pkts_weight,v", boost::program_options::value<double>(&valPktsFeature), "Weight of ATLAS_FEATURE_VALID_PACKETS feature (between 0.01 and 0.99). Sum of all three system features must be equal to 1.");

    try {
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv)
                                      .options(desc).run(), vm);
        
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            exit(1);
        }

        boost::program_options::notify(vm);
    
        /* Scenario number validation */
        if (scenarioNo < 1 || scenarioNo > 7) {
            std::cout << "ERROR: Invalid scenario number" << std::endl;
            std::cout << desc << std::endl;
            exit(1);
        }    

        switch (scenarioNo) {    
            case 7:
                /* System features weight in device reputation computation */
                if (sysFeatWeight < 0.01 || sysFeatWeight > 0.99) {
                    std::cout << "ERROR: Invalid value for a overall system features weight in device reputation computation" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                }
                /* System features weights validation */
                if ((regTimeFeature < 0.01 || regTimeFeature > 0.99) || (kaPktsFeature < 0.01 || kaPktsFeature > 0.99) || (valPktsFeature < 0.01 || valPktsFeature > 0.99)) {
                    std::cout << "ERROR: Invalid value for a system feature weight" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } else 
                    if ((regTimeFeature + kaPktsFeature + valPktsFeature) != 1.0) {
                        std::cout << "ERROR: Sum of system feature weight is not equal to 1" << std::endl;
                        std::cout << desc << std::endl;
                        exit(1);
                    }        
            case 1: case 2: case 3: case 4:
                /* Features number validation */
                if (noOfFeatures < 1 || noOfFeatures > 5) {
                    std::cout << "ERROR: Invalid number of features" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } 
            case 5:
                /* Client number validation */
                if (noOfClients < 1 || noOfClients > 10) {
                    std::cout << "ERROR: Invalid number of clients" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                }    
                /* Data refence value validation */
                if (badFeedbackProbab < 1 || badFeedbackProbab > 99) {
                    std::cout << "ERROR: Invalid value for data reference" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } 
                /* Threshold value validation */
                if (thresholdValue < 0.1 || thresholdValue > 0.95) {
                    std::cout << "ERROR: Invalid value for feedback threshold" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } 
                /* Client number validation */
                if (noOfIterations < 1 || noOfIterations > 50000) {
                    std::cout << "ERROR: Invalid number of iterations" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                }
                break;
            case 6: 
                /* Client number validation */
                if (noOfClients < 1 || noOfClients > 10) {
                    std::cout << "ERROR: Invalid number of clients" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                }    
                /* Data refence value validation */
                if (badFeedbackProbab < 1 || badFeedbackProbab > 99) {
                    std::cout << "ERROR: Invalid value for data reference" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } 
                /* Threshold value validation */
                if (thresholdValue < 0.1 || thresholdValue > 0.95) {
                    std::cout << "ERROR: Invalid value for feedback threshold" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } 
                /* Client number validation */
                if (noOfIterations < 1 || noOfIterations > 50000) {
                    std::cout << "ERROR: Invalid number of iterations" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                }                
                /* System features weights validation */
                if ((regTimeFeature < 0.01 || regTimeFeature > 0.99) || (kaPktsFeature < 0.01 || kaPktsFeature > 0.99) || (valPktsFeature < 0.01 || valPktsFeature > 0.99)) {
                    std::cout << "ERROR: Invalid value for a system feature weight" << std::endl;
                    std::cout << desc << std::endl;
                    exit(1);
                } else 
                    if ((regTimeFeature + kaPktsFeature + valPktsFeature) != 1.0) {
                        std::cout << "ERROR: Sum of system feature weight is not equal to 1" << std::endl;
                        std::cout << desc << std::endl;
                        exit(1);
                    }
                break;
        }
        

    } catch(boost::program_options::error& e) {
        std::cout << desc << std::endl;
        exit(1);
    }
}

int main(int argc, char **argv)
{
    parse_options(argc, argv);

    atlas::initLog();

    switch (scenarioNo)
    {
        case 1:
            std::cout << "Scenario 1 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_1(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 2:
            std::cout << "Scenario 2 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_2(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 3:
            std::cout << "Scenario 3 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_3(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 4:
            std::cout << "Scenario 4 with " << noOfClients << " clients, each with " << noOfFeatures << " features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_4(noOfClients, noOfFeatures, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 5:
            std::cout << "Scenario 5 with " << noOfClients << " clients, each with all Control plane features. Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_5(noOfClients, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 6:
            std::cout << "Scenario 6 with " << noOfClients << " clients, each with all Control plane features (weights are: " << regTimeFeature << ", " << kaPktsFeature << ", " << valPktsFeature << "). Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_6(noOfClients, regTimeFeature, kaPktsFeature, valPktsFeature, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
        case 7:
            std::cout << "Scenario 7 with " << noOfClients << " clients, each with " << noOfFeatures << " Data plane features and all Control plane features (weights are: " << regTimeFeature << ", " << kaPktsFeature << ", " << valPktsFeature << "). Bad_feedback_prob = " << badFeedbackProbab << ". Threshold = " << thresholdValue << ". Iterations = " << noOfIterations << std::endl;
            atlas::AtlasReputationTester::simulateScenario_7(noOfClients, noOfFeatures, sysFeatWeight, regTimeFeature, kaPktsFeature, valPktsFeature, badFeedbackProbab, thresholdValue, noOfIterations);
            break;
    }

    return 0;
};