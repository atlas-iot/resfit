#include "AtlasFirewallStats.h"


namespace atlas {

namespace {

/* JSON firewall stat - rule droppedPkts key */
const std::string ATLAS_FIREWALLSTATS_RULE_DROPPEDPKTS_JSON_KEY = "firewallRuleDroppedPkts";

/* JSON firewall stat - rulepassedPkts key */
const std::string ATLAS_FIREWALLSTATS_RULE_PASSEDPKTS_JSON_KEY = "firewallRulePassedPkts";

/* JSON firewall stat - TX droppedPkts key */
const std::string ATLAS_FIREWALLSTATS_TX_DROPPEDPKTS_JSON_KEY = "firewallTxDroppedPkts";

/* JSON firewall stat - TX passedPkts key */
const std::string ATLAS_FIREWALLSTATS_TX_PASSEDPKTS_JSON_KEY = "firewallTxPassedPkts";

} // anonymous namespace

AtlasFirewallStats& AtlasFirewallStats::operator=(const AtlasFirewallStats& c)
{
    clientId_ = c.clientId_;
    ruleDroppedPkts_ = c.ruleDroppedPkts_;
    rulePassedPkts_ = c.rulePassedPkts_;
    txDroppedPkts_ = c.txDroppedPkts_;
    txPassedPkts_ = c.txPassedPkts_;
    
    return *this;
}

bool AtlasFirewallStats::operator==(const AtlasFirewallStats& c)
{
    if (clientId_ != c.clientId_)
        return false;

    if (ruleDroppedPkts_ != c.ruleDroppedPkts_)
        return false;

    if (rulePassedPkts_ != c.rulePassedPkts_)
        return false;

    if (txDroppedPkts_ != c.txDroppedPkts_)
        return false;

    if (txPassedPkts_ != c.txPassedPkts_)
        return false;

    return true;
}

std::string AtlasFirewallStats::toJSON()
{
    return "\"" + ATLAS_FIREWALLSTATS_RULE_DROPPEDPKTS_JSON_KEY + "\": \"" + std::to_string(ruleDroppedPkts_) + "\","
            + "\n\"" + ATLAS_FIREWALLSTATS_RULE_PASSEDPKTS_JSON_KEY + "\": \"" + std::to_string(rulePassedPkts_) + "\","
            + "\n\"" + ATLAS_FIREWALLSTATS_TX_DROPPEDPKTS_JSON_KEY + "\": \"" + std::to_string(txDroppedPkts_) + "\","
            + "\n\"" + ATLAS_FIREWALLSTATS_TX_PASSEDPKTS_JSON_KEY + "\": \"" + std::to_string(txPassedPkts_) + "\"";
}

} // namespace atlas
