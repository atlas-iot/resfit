#include "AtlasRegistrationFeedback.h"
#include "../AtlasDeviceManager.h"
#include "../../logger/AtlasLogger.h"

namespace atlas {

double AtlasRegistrationFeedback::getFeedback()
{
    int regIntervalSec = device_.getRegInterval();
    
    /* Compute the registration time percentage */
    double feedback = (double) (regIntervalSec - prevRegIntervalSec_) / (double) samplePeriodSec_; 

    prevRegIntervalSec_ = regIntervalSec;

    ATLAS_LOGGER_INFO("Registration time feedback was computed for device with identity " + device_.getIdentity() + " to value " + std::to_string(feedback));

    return feedback;
}

} // namespace atlas
