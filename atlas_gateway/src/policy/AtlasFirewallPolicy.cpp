#include "AtlasFirewallPolicy.h"

namespace atlas {

namespace {

/* JSON policy-cliendId key */
const std::string ATLAS_POLICY_CLIENTID_JSON_KEY = "pubSubClientId";

/* JSON policy-qos key */
const std::string ATLAS_POLICY_QOS_JSON_KEY = "firewallPolicyQos";

/* JSON policy-ppm key */
const std::string ATLAS_POLICY_PPM_JSON_KEY = "firewallPolicyPpm";

/* JSON policy-payloadLen key */
const std::string ATLAS_POLICY_PAYLOADLEN_JSON_KEY = "firewallPolicyPayloadLen";

} // anonymous namespace

AtlasFirewallPolicy& AtlasFirewallPolicy::operator=(const AtlasFirewallPolicy& c)
{
    clientId_ = c.clientId_;
    qos_ = c.qos_;
    ppm_ = c.ppm_;
    payloadLen_ = c.payloadLen_;
    return *this;
}

bool AtlasFirewallPolicy::operator==(const AtlasFirewallPolicy& c)
{
    if((clientId_ == c.clientId_) && (qos_ == c.qos_) && (ppm_ = c.ppm_) && (payloadLen_ == c.payloadLen_))
        return true;
    else
        return false;
}

std::string AtlasFirewallPolicy::toJSON()
{
    return  "\"" + ATLAS_POLICY_CLIENTID_JSON_KEY + "\": \"" + clientId_ + "\","
            + "\n\"" + ATLAS_POLICY_QOS_JSON_KEY + "\": \"" + std::to_string(qos_) + "\","
            + "\n\"" + ATLAS_POLICY_PPM_JSON_KEY + "\": \"" + std::to_string(ppm_) + "\","
            + "\n\"" + ATLAS_POLICY_PAYLOADLEN_JSON_KEY + "\": \"" + std::to_string(payloadLen_) + "\"";
}

} // namespace atlas
