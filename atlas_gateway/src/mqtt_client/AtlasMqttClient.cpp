#include "AtlasMqttClient.h"
#include "../logger/AtlasLogger.h"

namespace atlas
{

AtlasMqttClient::AtlasMqttClient() : client_(nullptr), cb_(nullptr),
                                     connTok_(nullptr), discTok_(nullptr),
                                     pubTok_(nullptr) {}

AtlasMqttClient& AtlasMqttClient::getInstance()
{
    static AtlasMqttClient instance;

    return instance;
}

bool AtlasMqttClient::initConnection(const std::string &address, const std::string &clientID,
                                     const std::string &username, const std::string &password,
                                     const std::string &certFile)
{
    mqtt::connect_options connOps;
    mqtt::ssl_options sslopts;

    ATLAS_LOGGER_INFO("Initializing client (" + clientID + ") connection to server (" + address + ")");

    if (client_) {
        ATLAS_LOGGER_DEBUG("MQTT client is already initialized!");
        return true;
    }

    /* Creating the async_client */
    try {
        client_ = new mqtt::async_client(address, clientID);            
    } catch(const std::exception& e) {
        ATLAS_LOGGER_ERROR(std::string(e.what()));
        return false;
    }        

    /* Set SSL options */
    sslopts.set_trust_store(certFile);
    
    /* Set connection options */
    connOps.set_user_name(username);
    connOps.set_password(password);
    connOps.set_ssl(sslopts);
    connOps.set_clean_session(true);
    
    /* Create callback */
    cb_ = new AtlasMqttClient_callback(*client_, connOps);
    client_->set_callback(*cb_);

    /* Try to establish a connection */
    connect(connOps);

    return true;
} 

void AtlasMqttClient::connect(mqtt::connect_options &connOps)
{
    ATLAS_LOGGER_INFO("Connecting to cloud back-end");

    /* Connecting to remote server */
    try {
        connTok_ = client_->connect(connOps, nullptr, *cb_);
        ATLAS_LOGGER_INFO("Connection to cloud back-end will be established.");
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_connect: " + std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
    }
}


bool AtlasMqttClient::tryPublishMessage(const std::string &topic, const std::string &message, int qos)
{
    try {   
        if (!connTok_) {
            ATLAS_LOGGER_ERROR("No existing connection for client [" + client_->get_client_id() + "]. PUBLISH aborted.");
            return false;
        } else {
            if (!client_->is_connected()) { //non-blocking task --> exit with false and wait for a re-call of publish            
                ATLAS_LOGGER_ERROR("Previous CONNECT action has not yet finished for client [" + client_->get_client_id() + "]. PUBLISH should be delayed.");
                return false;
            } else {
                mqtt::message_ptr pubMsg = mqtt::make_message(topic, message);
                pubMsg->set_qos(qos);                
                pubTok_ = client_->publish(pubMsg, nullptr, deliveryActList_);  
                ATLAS_LOGGER_INFO("Message [" + std::to_string(pubTok_->get_message_id()) + "] will be sent with QoS " + 
                            std::to_string(qos) + " by client [" + client_->get_client_id() + "]."); 
            }
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_tryPublish: " + std::string(e.what()));
        return false;
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        return false;
    }

    return true;
}


bool AtlasMqttClient::subscribeTopic(const std::string &topic, int QoS)
{
    try {
        if (client_->is_connected()) {     
            ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] subscribing to topic [" + topic + "] with QoS " + std::to_string(QoS));
            client_->subscribe(topic, QoS, nullptr, subscribeActList_);
        } else {
            ATLAS_LOGGER_ERROR("No active connection. SUBSCRIBE aborted.");    
            return false;        
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_subscribe: " + std::string(e.what()));
        return false;
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
        return false;
    }

    return true;
}

void AtlasMqttClient::disconnect()
{
    try {
        if (client_->is_connected()) {
            if (pubTok_  && !pubTok_->is_complete())
                pubTok_->wait();
                       
            discTok_ = client_->disconnect(nullptr, disconnectActList_);
            ATLAS_LOGGER_INFO("Client [" + client_->get_client_id() + "] has successfully disconnected from " + client_->get_server_uri());
        }
    } catch(const mqtt::exception& e) {
        ATLAS_LOGGER_ERROR("Exception caught in AtlasMqttClient_disconnect: " + std::string(e.what()));
    } catch(const char* e) {
        ATLAS_LOGGER_ERROR(std::string(e));
    }  
}

void AtlasMqttClient::addConnectionCb(IAtlasMqttState *connCb)
{
    /* If callback is invalid or client callback is not created yet */
    if (!connCb || !cb_)
        return;

    cb_->addConnectionCb(connCb);
}

void AtlasMqttClient::removeConnectionCb(IAtlasMqttState *connCb)
{
    /* If callback is invalid or client callback is not created yet */
    if (!connCb || !cb_)
        return;
    
    cb_->removeConnectionCb(connCb);
}

AtlasMqttClient::~AtlasMqttClient()
{
    this->disconnect();
    discTok_->wait();
    delete client_;

    delete cb_;
    cb_ = nullptr;
}

} //namespace atlas
