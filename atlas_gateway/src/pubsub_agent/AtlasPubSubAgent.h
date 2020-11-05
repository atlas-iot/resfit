#ifndef __ATLAS_PUBSUB_AGENT_H__
#define __ATLAS_PUBSUB_AGENT_H__

#include <stdint.h>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../utils/AtlasUtils.h"
#include "../policy/AtlasFirewallPolicy.h"
#include "../commands/AtlasCommandXfer.h"

namespace atlas {

class AtlasPubSubAgent
{

public:

    /**
     * @brief Get singleton instance
     * @return Singleton instance
     */
    static AtlasPubSubAgent& getInstance();

    /**
    * @brief Dtor for publish-subscribe agent
    * @return none
    */
    ~AtlasPubSubAgent();

    /**
    * @brief Start publish-subscribe agent
    * @return none
    */
    void start();

    /**
    * @brief Install firewall rule
    * @param[in] clientId Publish-subscribe Client id
    * @param[in] device identity
    * @param[in] policy qos
    * @param[in] policy ppm
    * @param[in] policy payloadLen
    * @return none
    */
    void installFirewallRule(const std::string &identity, const AtlasFirewallPolicy *policy);

    /**
    * @brief Remove firewall rule for client id
    * @param[in] clientId Publish-subscribe Client id
    * @return none
    */
    void removeFirewallRule(const std::string &clientId);
    
    /**
    * @brief Get firewall rule statistics for client id
    * @param[in] clientId Publish-subscribe Client id
    * @return none
    */
    void getFirewallRuleStats(const std::string &clientId);

    AtlasPubSubAgent(const AtlasPubSubAgent&) = delete;
    AtlasPubSubAgent& operator=(const AtlasPubSubAgent&) = delete;

private:

    /**
    * @brief Ctor for publish-subscribe agent
    * @return none
    */
    AtlasPubSubAgent();

    /**
    * @brief Write data to publish-subscribe agent
    * @param[in] buf Data buffer
    * @param[in] bufLen Data buffer length
    * @return none
    */
    void write(const uint8_t *buf, size_t bufLen);

    /**
    * @brief Process firewall rule statistics
    * @param[in] cmdBuf Firewall rule statistics command buffer
    * @param[in] cmdLen Command length
    * @return none
    */
    void processFirewallRuleStat(const uint8_t *cmdBuf, uint16_t cmdLen);
    
    /**
    * @brief Get all firewall rules for publish-subscribe agent
    * @return none
    */
    void getAllFirewallRules();

    /**
    * @brief Process command received from publish-subsribe agent
    * @param[in] cmdBUffer Command buffer
    * @param[in] cmdLen Command length
    * @return none
    */
    void processCommand(const uint8_t *cmdBuffer, size_t cmdLen);
    
    /**
    * @brief Accept connections from publish-subscribe agent
    * @param[in] error Connection error
    * @return none
    */
    void handleAccept(const boost::system::error_code& error);
    
    /**
    * @brief Read data from publish-subscribe agent
    * @param[in] error Read error
    * @param[in] bytesTransferred Number of bytes transferred
    * @return none
    */
    void handleRead(const boost::system::error_code& error, size_t bytesTransferred);
    
    /**
    * @brief Write data to publish-subscribe agent
    * @param[in] error Write error
    * @return none
    */
    void handleWrite(const boost::system::error_code& error);

    /* Publish-subscribe agent server */
    boost::asio::local::stream_protocol::acceptor acceptor_;
    
    /* Publish-subscribe agent accepting socket */
    boost::asio::local::stream_protocol::socket *acceptingSocket_;
    
    /* Publish-subscribe agent connecting socket */
    boost::asio::local::stream_protocol::socket *connectedSocket_;

    /* Command transfer over stream channel */
    AtlasCommandXfer cmdXfer_;
};

} // namespace atlas

#endif /* __ATLAS_PUBSUB_AGENT_H__ */
