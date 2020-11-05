#ifndef __ATLAS_MQTT_CLIENT_ACTION_LISTENER_H__
#define __ATLAS_MQTT_CLIENT_ACTION_LISTENER_H__

#include <mqtt/async_client.h>
#include <stdlib.h>
#include <atomic>

namespace atlas
{
//Types to identify different action listeners
enum class AtlasMqttClient_actionListenerTypes {
    Generic,
    Connect,
    Disconnect,
    Delivery
};

//Basic action listener
class AtlasMqttClient_actionListener : public virtual mqtt::iaction_listener
{
protected:
    void on_failure(const mqtt::token&) override;
    void on_failure(const AtlasMqttClient_actionListenerTypes&);

    void on_success(const mqtt::token&) override;
    void on_success(const AtlasMqttClient_actionListenerTypes&);
private:
    AtlasMqttClient_actionListenerTypes actionListenerType_;
};

//Action listener for connection events
class AtlasMqttClient_connectActionListener : public AtlasMqttClient_actionListener
{
public:
    AtlasMqttClient_connectActionListener() : done_(false) {}
    bool is_done() const { return done_; }

private:
    std::atomic<bool> done_;
    void on_failure(const mqtt::token&) override;
    void on_success(const mqtt::token&) override;

};

//Action listener for publish events
class AtlasMqttClient_deliveryActionListener : public AtlasMqttClient_actionListener
{
public:
    AtlasMqttClient_deliveryActionListener() : done_(false) {}
    bool is_done() const { return done_; }

private:
    std::atomic<bool> done_;
    void on_failure(const mqtt::token&) override;
    void on_success(const mqtt::token&) override;
};


//Action listener for disconnecting events
class AtlasMqttClient_disconnectActionListener : public AtlasMqttClient_actionListener
{
public:
    AtlasMqttClient_disconnectActionListener() : done_(false) {}
    bool is_done() const { return done_; }
    
private:
    std::atomic<bool> done_;
    void on_failure(const mqtt::token&) override;
    void on_success(const mqtt::token&) override;
};


//Action listener for subscribe events
class AtlasMqttClient_subscriptionActionListener : public virtual mqtt::iaction_listener
{
public:
    AtlasMqttClient_subscriptionActionListener() : done_(false) {}
    bool is_done() const { return done_; }
    
private:
    std::atomic<bool> done_;
    void on_failure(const mqtt::token&) override;
    void on_success(const mqtt::token&) override;
};

} //namespace atlas

#endif /*__ATLAS_MQTT_CLIENT_ACTION_LISTENER_H__*/