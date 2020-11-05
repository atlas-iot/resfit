#ifndef __ATLAS_KEEPALIVE_FEEDBACK_H__
#define __ATLAS_KEEPALIVE_FEEDBACK_H__

#include "IAtlasFeedback.h"

namespace atlas {

class AtlasKeepaliveFeedback: public IAtlasFeedback
{
public:
    /**
    * @brief Ctor for keep-alive feedback
    * @param[in] device Client device
    * @param[in] sampleKeepalivePkts Total (max) number of keep-alive packets per sample window
    * @return none
    */
    AtlasKeepaliveFeedback(AtlasDevice &device,
                           int sampleKeepalivePkts) : IAtlasFeedback(device,
                                                                    AtlasDeviceFeatureType::ATLAS_FEATURE_KEEPALIVE_PACKETS),
                                                      sampleKeepalivePkts_(sampleKeepalivePkts),
                                                      prevKeepalivePkts_(0) {}

    /**
    * @brief Get feedback for a device
    * @return Feedback value
    */
    double getFeedback();

private:
    /* Holds the total number of keep-alive packets per sample window */
    int sampleKeepalivePkts_;

    /* Keep-alive packets from the previous sample */
    int prevKeepalivePkts_;
};

} // namespace atlas

#endif /* __ATLAS_KEEPALIVE_FEEDBACK_H__ */
