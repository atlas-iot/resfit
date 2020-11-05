#include "AtlasGateway.h"
#include "../sql/AtlasSQLite.h"

namespace atlas {

namespace {
    /* JSON register event key */
    const std::string ATLAS_GATEWAY_OWNER_JSON_KEY = "owner";
} // anonymous namespace

AtlasGateway::AtlasGateway(std::shared_ptr<AtlasDeviceCloud> deviceCloud) : claimed_(false),
                                                                            deviceCloud_(deviceCloud)
{
    /* Get claimed information from database */
    bool result = AtlasSQLite::getInstance().selectOwnerInfo(ownerSecretKey_, ownerIdentity_);
    if(result && ownerSecretKey_ != "" && ownerIdentity_ != "") {
        ATLAS_LOGGER_INFO("Gateway is claimed by owner with identity: " + ownerIdentity_);
        claimed_ = true;
    } else {
        ATLAS_LOGGER_ERROR("Cannot get owner (claim) information from database");
    }
}

bool AtlasGateway::claimGateway(const std::string &ownerIdentity,
                                const std::string &ownerSecretKey)
{
    /* Save owner secret key and identity into the database */
    bool result = AtlasSQLite::getInstance().insertOwner(ownerSecretKey,
                                                         ownerIdentity);
    if(!result) {
        ATLAS_LOGGER_ERROR("Cannot insert claim information into database");
        return false;
    }

    /* Mark the gateway as claimed */
    claimed_ = true;

    ownerIdentity_ = ownerIdentity;
    ownerSecretKey_ = ownerSecretKey;

    /* Update claim information on the cloud */
    deviceCloud_->updateGateway(ownerToJSON());

    return true;
}

std::string AtlasGateway::ownerToJSON() const
{
    return "\"" + ATLAS_GATEWAY_OWNER_JSON_KEY + "\": \"" + ownerIdentity_ + "\"";
}

std::string AtlasGateway::toJSON() const
{
    return ownerToJSON();
}

} // namespace atlas
