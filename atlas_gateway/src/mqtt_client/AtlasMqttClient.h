#ifndef __ATLAS_MQTT_CLIENT_H__
#define __ATLAS_MQTT_CLIENT_H__

#include <stdlib.h>
#include <string>
#include <mqtt/async_client.h>
#include "AtlasMqttClient_callback.h"
#include "AtlasMqttClient_actionListener.h"
#include "IAtlasMqttState.h"

namespace atlas
{
class AtlasMqttClient
{
public:
    /**
     * @brief Get MQTT client instance
     * @return MQTT client instance
    */
    static AtlasMqttClient& getInstance();

    /**
     * @brief Default Dtor for MQTT client
     * @return none 
    */
    ~AtlasMqttClient();

    /**
     * @brief Init connection to Atlas Cloud module with explicit parameters
     * @param[in] IP address or hostname of the Atlas Cloud module
     * @param[in] clientID MQTT client id
     * @param[in] username MQTT username
     * @param[in] password MQTT password
     * @param[in] certFile Mosquitto certificate file
     * @return none
    */
    bool initConnection(const std::string &address, const std::string &clientID,
                        const std::string &username, const std::string &password,
                        const std::string &certFile);

    /**
     * @brief Subscribe to a topic on the Atlas Cloud module
     * @param[in] Topic to subscribe to
     * @param[in] Desired QoS
     * @return True on success, False otherwise
    */
    bool subscribeTopic(const std::string &topic, int qos = 2);

    /**
     * @brief Disconnect from a Atlas Cloud module
     * @return none
    */
    void disconnect();

    /**
     * @brief Non-blocking Publish of a message to a specific topic
     * @param[in] Topic to publish the message to
     * @param[in] Message to be published
     * @param[in] QoS for message publishing
     * @return True on publish succes, False otherwise 
    */
    bool tryPublishMessage(const std::string &topic, const std::string &message, int qos = 2);

    /**
    * @brief Add connection state callback
    * @param[in] connCb COnnection state callback
    * @return none
    */
    void addConnectionCb(IAtlasMqttState *connCb);
    
     /**
    * @brief Remove connection state callback
    * @param[in] connCb Connection state callback
    * @return none
    */
    void removeConnectionCb(IAtlasMqttState *connCb);

    AtlasMqttClient(const AtlasMqttClient &) = delete;
    AtlasMqttClient & operator = (const AtlasMqttClient &) = delete;

private:	
    /* MQTT client */
    mqtt::async_client *client_;
    
    /* MQTT client callback */
    AtlasMqttClient_callback *cb_;
   
    /* MQTT connect token */ 
    mqtt::token_ptr connTok_;
    
    /* MQTT disconnect token */
    mqtt::token_ptr discTok_;
   
    /* MQTT publish token */ 
    mqtt::delivery_token_ptr pubTok_;
    
    AtlasMqttClient_connectActionListener connectActList_;
    AtlasMqttClient_deliveryActionListener deliveryActList_;
    AtlasMqttClient_disconnectActionListener disconnectActList_;
    AtlasMqttClient_subscriptionActionListener subscribeActList_;

    /**
     * @brief Default ctor for MQTT client
     * @return none
    */
    AtlasMqttClient();

    /**
     * @brief Connect to cloud back-end
     * @param[in] connOps Connection options
     * @return none
    */
    void connect(mqtt::connect_options &connOps);
};

} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_H__*/
