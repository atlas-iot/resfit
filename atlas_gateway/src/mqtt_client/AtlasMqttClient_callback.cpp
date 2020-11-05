#include "AtlasMqttClient_callback.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"
#include "../cloud/AtlasCloudCmdParser.h"

#include <iostream>
#include <thread>

namespace atlas
{

namespace {
    const int ATLAS_MQTT_RECONNECT_INTERVAL_MS = 5000;
} // anonymous namespace

AtlasMqttClient_callback::AtlasMqttClient_callback(mqtt::async_client& client,
                                                   mqtt::connect_options& connOpts) : client_(client), connOpts_(connOpts),
                                                                                      alarm_("AtlasMqttClientReconnect",
                                                                                             ATLAS_MQTT_RECONNECT_INTERVAL_MS, true,
                                                                                             boost::bind(&AtlasMqttClient_callback::alarmCallback, this)) {}

void AtlasMqttClient_callback::connection_lost(const std::string& cause)
{
    AtlasScheduler::getInstance().getService().post([this]() 
                                                           {
                                                               ATLAS_LOGGER_INFO("AtlasMqttClient_Callback (connection_lost): Connection lost. Trying to reconnect...");
                                                               for (auto it = connCb_.begin(); it != connCb_.end(); ++it)
                                                                   (*it)->onDisconnect();
                                                               
                                                               this->alarm_.start();
                                                           });
}

void AtlasMqttClient_callback::message_arrived(mqtt::const_message_ptr msg)
{
    ATLAS_LOGGER_INFO("AtlasMqttClient_Callback (message_arrived): Message arrived on topic [" + msg->get_topic() + "] with payload: \"" + msg->to_string() + "\"");

    AtlasScheduler::getInstance().getService().post(boost::bind(&AtlasCloudCmdParser::parseCmd, &AtlasCloudCmdParser::getInstance(), msg->to_string()));
}

void AtlasMqttClient_callback::alarmCallback()
{
    ATLAS_LOGGER_INFO("AtlasMqttClient_Callback reconnect alarm");

    try {
        client_.connect(connOpts_, nullptr, *this);
    } catch (const mqtt::exception& e) {
        ATLAS_LOGGER_DEBUG("AtlasMqttClient_Callback (reconnect): Error on reconnect --> what(): " + std::string(e.what()));
    }
}

void AtlasMqttClient_callback::on_failure(const mqtt::token& tok)
{
    AtlasScheduler::getInstance().getService().post([this]() 
                                                           {
                                                               ATLAS_LOGGER_INFO("AtlasMqttClient_Callback (connection_lost): Connection lost. Trying to reconnect...");
                                                               for (auto it = connCb_.begin(); it != connCb_.end(); ++it)
                                                                   (*it)->onDisconnect();
                                                               
                                                               this->alarm_.start();
                                                           });
}

void AtlasMqttClient_callback::on_success(const mqtt::token&)
{
    AtlasScheduler::getInstance().getService().post([this]() 
                                                           {
                                                               ATLAS_LOGGER_INFO("AtlasMqttClient_Callback (on_success): Gateway is connected to the cloud broker!");
                                                               for (auto it = connCb_.begin(); it != connCb_.end(); ++it)
                                                                   (*it)->onConnect();
                                                           });
}

void AtlasMqttClient_callback::addConnectionCb(IAtlasMqttState *connCb)
{
    ATLAS_LOGGER_DEBUG("Add a connection state listener");

    connCb_.push_back(connCb);
}

void AtlasMqttClient_callback::removeConnectionCb(IAtlasMqttState *connCb)
{
    ATLAS_LOGGER_DEBUG("Remove a connection state listener");
    
    for (auto it = connCb_.begin(); it != connCb_.end(); ++it)
        if (*it == connCb) {
            ATLAS_LOGGER_DEBUG("Connection state listener found for removal");
            connCb_.erase(it);
            break;
        }
}

} //namespace atlas
