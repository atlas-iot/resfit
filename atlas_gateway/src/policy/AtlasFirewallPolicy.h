#ifndef __ATLAS_FIREWALL_POLICY_H__
#define __ATLAS_FIREWALL_POLICY_H__

#include <string>


namespace atlas {

class AtlasFirewallPolicy
{

public:
    /**
    * @brief Default ctor for firewall policy
    * @return none
    */
    AtlasFirewallPolicy() : clientId_("") {}

    /**
    * @brief Overwrite = operator
    * @param[in] AtlasFirewallPolicy reference
    * @return referece to this
    */
    AtlasFirewallPolicy& operator=(const AtlasFirewallPolicy& c);

    /**
    * @brief Overwrite == operator
    * @param[in] AtlasFirewallPolicy reference
    * @return true or false
    */
    bool operator==(const AtlasFirewallPolicy& c);

    /**
    * @brief Get policy clientId
    * @return clientId
    */
    inline std::string getClientId() const { return clientId_; }

    /**
    * @brief Get policy qos
    * @return qos
    */
    inline uint16_t getQOS() const { return qos_; }

    /**
    * @brief Get policy ppm
    * @return ppm
    */
    inline uint16_t getPPM() const { return ppm_; }

    /**
    * @brief Get policy payloadLen
    * @return payloadLen
    */
    inline uint16_t getPayloadLen() const { return payloadLen_; }

    /**
    * @brief Set policy clientId
    * @param[in] client id
    * @return none
    */
    inline void setClientId(const std::string &clientId) { clientId_ = clientId; }

    /**
    * @brief Set policy qos
    * @param[in] qos
    * @return none
    */
    inline void setQOS(uint16_t qos) { qos_ = qos; }

    /**
    * @brief Set policy ppm
    * @param[in] ppm
    * @return none
    */
    inline void setPPM(uint16_t ppm) { ppm_ = ppm; }

    /**
    * @brief Set policy payloadLen
    * @param[in] payloadLen
    * @return none
    */
    inline void setPayloadLen(uint16_t payloadLen) { payloadLen_ = payloadLen; }
    
    /**
    * @brief Put all firewall policy info in json format
    * @return string
    */
    std::string toJSON();

private:
    /* Policy info - clientid*/
    std::string clientId_;

    /* Policy info - qos*/
    uint16_t qos_;

    /* Policy info - ppm*/
    uint16_t ppm_;

    /* Policy info - payloadLen*/
    uint16_t payloadLen_;
};

} // namespace atlas

#endif /* __ATLAS_FIREWALL_POLICY_H__ */
