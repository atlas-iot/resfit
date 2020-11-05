#ifndef __IATLAS_FEEDBACK_H__
#define __IATLAS_FEEDBACK_H__

#include "../AtlasDevice.h"
#include "../../reputation_impl/AtlasDeviceFeatureType.h"

namespace atlas {

class IAtlasFeedback
{
public:
    /**
    * @brief Ctor for feedback interface
    * @param[in] device Client device
    * @param[in] type Reputation feature type
    * @return none
    */
    IAtlasFeedback(AtlasDevice &device, AtlasDeviceFeatureType type) : device_(device), type_(type) {}

    /**
    * @brief Get reputation feature type
    * @return Reputation feature type
    */
    inline AtlasDeviceFeatureType getType() const { return type_; }

    /**
    * @brief Get feedback for a device
    * @return Feedback value
    */
    virtual double getFeedback() = 0;
    
    /**
    * @brief Dtor for feedback interface
    * @return none
    */
    virtual ~IAtlasFeedback() {};

protected:
    /* Client device */
    AtlasDevice &device_;

private:
    /* Reputation feature type */
    AtlasDeviceFeatureType type_;
};

} // namespace atlas

#endif /* __IATLAS_FEEDBACK_H__ */
