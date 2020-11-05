#ifndef __ATLAS_REGISTRATION_FEEDBACK_H__
#define __ATLAS_REGISTRATION_FEEDBACK_H__

#include "IAtlasFeedback.h"

namespace atlas {

class AtlasRegistrationFeedback: public IAtlasFeedback
{
public:
    /**
    * @brief Ctor for registration feedback
    * @param[in] device Client device
    * @param[in] regSamplePeriod Registration sample period (sampling window in seconds)
    * @return none
    */
    AtlasRegistrationFeedback(AtlasDevice &device,
                              int samplePeriod) : IAtlasFeedback(device,
                                                                    AtlasDeviceFeatureType::ATLAS_FEATURE_REGISTER_TIME),
                                                  samplePeriodSec_(samplePeriod),
                                                  prevRegIntervalSec_(0) {}

    /**
    * @brief Get feedback for a device
    * @return Feedback value
    */
    double getFeedback();

private:
    /* Holds the registration sample period */
    int samplePeriodSec_;

    /* Registration interval from the previous sample */
    int prevRegIntervalSec_;
};

} // namespace atlas

#endif /* __ATLAS_REGISTRATION_FEEDBACK_H__ */
