#ifndef __ATLAS_PACKETS_FEEDBACK_H__
#define __ATLAS_PACKETS_FEEDBACK_H__

#include "IAtlasFeedback.h"
#include "../../statistics/AtlasFirewallStats.h"

namespace atlas {

class AtlasPacketsFeedback: public IAtlasFeedback
{
public:
    /**
    * @brief Ctor for TX packets feedback
    * @param[in] device Client device
    * @return none
    */
    AtlasPacketsFeedback(AtlasDevice &device) : IAtlasFeedback(device,
                                                               AtlasDeviceFeatureType::ATLAS_FEATURE_VALID_PACKETS) {}

    /**
    * @brief Get feedback for a device
    * @return Feedback value
    */
    double getFeedback();

private:

    /* TX packet statistics from previous sample */
    AtlasFirewallStats prevStats_;
};

} // namespace atlas

#endif /* __ATLAS_PACKETS_FEEDBACK_H__ */
