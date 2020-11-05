#ifndef __ATLAS_FIREWALL_STATS_H__
#define __ATLAS_FIREWALL_STATS_H__

#include <string>
#include "../pubsub_agent/AtlasPubSubAgent.h"

namespace atlas {

class AtlasFirewallStats
{

public:
    /**
    * @brief Default ctor for firewall statistics
    * @return none
    */
    AtlasFirewallStats() : clientId_(""), ruleDroppedPkts_(0), rulePassedPkts_(0),
                           txDroppedPkts_(0), txPassedPkts_(0) {}

    /**
    * @brief Overwrite = operator
    * @param[in] AtlasFirewallStats reference
    * @return referece to this
    */
    AtlasFirewallStats& operator=(const AtlasFirewallStats& c);

    /**
    * @brief Overwrite == operator
    * @param[in] AtlasFirewallStats reference
    * @return true or false
    */
    bool operator==(const AtlasFirewallStats& c);

    /**
    * @brief Get firewall stats clientId
    * @return clientId
    */
    inline std::string getClientId() const { return clientId_; }

    /**
    * @brief Get TX dropped packets
    * @return droppedPkts
    */
    inline uint32_t getTxDroppedPkts() const { return txDroppedPkts_; }

    /**
    * @brief Get TX passed packets
    * @return passedPkts
    */
    inline uint32_t getTxPassedPkts() const { return txPassedPkts_; }

    /**
    * @brief Get firewall stats droppedPkts
    * @return droppedPkts
    */
    inline uint32_t getRuleDroppedPkts() const { return ruleDroppedPkts_; }

    /**
    * @brief Get firewall stats passedPkts
    * @return passedPkts
    */
    inline uint32_t getRulePassedPkts() const { return rulePassedPkts_; }

    /**
    * @brief Set firewall stats clientId
    * @param[in] clientId
    * @return none
    */
    inline void setClientId(const std::string &clientId) { clientId_ = clientId; }

    /**
    * @brief Set firewall stats droppedPkts
    * @param[in] droppedPkts
    * @return none
    */
    inline void addRuleDroppedPkts(uint32_t ruleDroppedPkts) { ruleDroppedPkts_ += ruleDroppedPkts; }

    /**
    * @brief Set firewall stats passedPkts
    * @param[in] passedPkts
    * @return none
    */
    inline void addRulePassedPkts(uint32_t rulePassedPkts) { rulePassedPkts_ += rulePassedPkts; }

    /**
    * @brief Set TX number of dropped packets
    * @param[in] txDroppedPackets TX number of dropped packets
    * @return none
    */
    inline void addTxDroppedPkts(uint32_t txDroppedPkts) { txDroppedPkts_ += txDroppedPkts; }

    /**
    * @brief Set TX number of passed packets
    * @param[in] txPassedPkts TX number of passed packets
    * @return none
    */
    inline void addTxPassedPkts(uint32_t txPassedPkts) { txPassedPkts_ += txPassedPkts; }

    /**
    * @brief Put all stats info in json format
    * @return string
    */
    std::string toJSON();

private:
    /* Policy clientid*/
    std::string clientId_;

    /* Firewall statistic param - droppedPkts (ingress) */
    uint32_t ruleDroppedPkts_;

    /* Firewall statistic param - passedPkts (ingress) */
    uint32_t rulePassedPkts_;

    /* TX dropped packets from the device with client id (egress) */
    uint32_t txDroppedPkts_;

    /* TX passed packets from the device with client id (egress) */
    uint32_t txPassedPkts_;
};

} // namespace atlas

#endif /* __ATLAS_FIREWALL_STATS_H__ */
