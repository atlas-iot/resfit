#ifndef __ATLAS_GATEWAY_H__
#define __ATLAS_GATEWAY_H__

#include <string>
#include "../cloud/AtlasDeviceCloud.h"

namespace atlas {

class AtlasGateway
{
public:
    AtlasGateway(std::shared_ptr<AtlasDeviceCloud> deviceCloud);

    /**
     * @brief Claim gateway
     * @param[in] ownerIdentity Owner identity
     * @param[in] ownerSecretKey Owner secret key
     * @return True if gateway was claimed, false otherwise
     */
    bool claimGateway(const std::string &ownerIdentity,
                      const std::string &ownerSecretKey);

    /**
     * @brief Indicates if the gateway is claimed
     * @return true if gateway is claimed, false otherwise
     */
    bool isClaimed() const { return claimed_; }

    /**
     * @brief Get owner secret key
     * @return Owner secret key
     */
    std::string getOwnerSecretKey() const { return ownerSecretKey_; }

    /**
     * @brief Get owner identity
     * @return Owner identity 
     */
    std::string getOwnerIdentity() const { return ownerIdentity_; }

    /**
     * @brief Serialize gateway information to JSON
     * @return JSON formatted gateway information
     */
    std::string toJSON() const;

private:
    /**
     * @brief Serialize owner information to JSON
     * @return JSON formatted owner information
     */
    std::string ownerToJSON() const;

    /* Indicates if gateway is claimed by an owner */
    bool claimed_;

    /* Holds the owner secret key */
    std::string ownerSecretKey_;

    /* Holds the owner identity */
    std::string ownerIdentity_;

  /* Cloud back-end manager */
    std::shared_ptr<AtlasDeviceCloud> deviceCloud_;
};

} // namespace atlas

#endif /* __ATLAS_GATEWAY_H__ */
