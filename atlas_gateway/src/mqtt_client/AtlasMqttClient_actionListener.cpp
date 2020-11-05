#include <string>
#include "AtlasMqttClient_actionListener.h"
#include "../logger/AtlasLogger.h"

namespace atlas
{

//***********Class base Action Listener*******************
void AtlasMqttClient_actionListener::on_failure(const mqtt::token& tok)
{
    std::string tmpLine = tok.get_client()->get_client_id() + "] for token [" + std::to_string(tok.get_message_id()) + "].";
    switch (actionListenerType_) {
        case AtlasMqttClient_actionListenerTypes::Connect:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Connect Listener: Failure of client [" +  tmpLine);
            break;
        case AtlasMqttClient_actionListenerTypes::Disconnect :
            ATLAS_LOGGER_INFO("AtlasMqttClient_Disconnect Listener: Failure of client [" +  tmpLine);
            break;
        case AtlasMqttClient_actionListenerTypes::Delivery:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Delivery Listener: Failure of client [" +  tmpLine);
            break;
        case AtlasMqttClient_actionListenerTypes::Generic:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Generic Listener: Failure of client [" +  tmpLine);
            break;
        default:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Unknown Listener: Failure of client [" + tmpLine);
            break;
    }    
}
void AtlasMqttClient_actionListener::on_failure(const AtlasMqttClient_actionListenerTypes& tip)
{
    actionListenerType_ = tip;
}


void AtlasMqttClient_actionListener::on_success(const mqtt::token& tok)
{
    std::string tmpLine = tok.get_client()->get_client_id() + "] for token [" + std::to_string(tok.get_message_id()) + "].";
    switch (actionListenerType_) {
        case AtlasMqttClient_actionListenerTypes::Connect:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Connect Listener: Success of client [" +  tmpLine);
            break;
        case AtlasMqttClient_actionListenerTypes::Disconnect :
            ATLAS_LOGGER_INFO("AtlasMqttClient_Disconnect Listener: Success of client [" +  tmpLine);
            break;
        case AtlasMqttClient_actionListenerTypes::Delivery:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Delivery Listener: Success of client [" +  tmpLine);
            break;
        case AtlasMqttClient_actionListenerTypes::Generic:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Generic Listener: Success of client [" +  tmpLine);
            break;
        default:
            ATLAS_LOGGER_INFO("AtlasMqttClient_Unknown Listener: Success of client [" + tmpLine);
            break;
    } 
}
void AtlasMqttClient_actionListener::on_success(const AtlasMqttClient_actionListenerTypes& tip)
{
    actionListenerType_ = tip;
}

//************Class Connecting Action Listener**************
void AtlasMqttClient_connectActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(AtlasMqttClient_actionListenerTypes::Connect);
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_connectActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(AtlasMqttClient_actionListenerTypes::Connect);
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}

//************Class Delivery Action Listener****************
void AtlasMqttClient_deliveryActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(AtlasMqttClient_actionListenerTypes::Delivery);
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_deliveryActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(AtlasMqttClient_actionListenerTypes::Delivery);
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}


//************Class Disconnecting Action Listener**************
void AtlasMqttClient_disconnectActionListener::on_failure(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_failure(AtlasMqttClient_actionListenerTypes::Disconnect);
    AtlasMqttClient_actionListener::on_failure(tok);
    done_ = true;
}

void AtlasMqttClient_disconnectActionListener::on_success(const mqtt::token& tok)
{
    AtlasMqttClient_actionListener::on_success(AtlasMqttClient_actionListenerTypes::Disconnect);
    AtlasMqttClient_actionListener::on_success(tok);
    done_ = true;
}


//*************Class Subscribe Action Listener******************
void AtlasMqttClient_subscriptionActionListener::on_failure(const mqtt::token& tok)
{
    std::string tmpMsg = "Subscription failure";
    if (tok.get_message_id() != 0)
        tmpMsg = tmpMsg + " for token: [" + std::to_string(tok.get_message_id()) + "]";
    ATLAS_LOGGER_INFO(tmpMsg);
    done_ = false;
}

void AtlasMqttClient_subscriptionActionListener::on_success(const mqtt::token& tok)
{
    std::string tmpMsg = "Subscription success";
    if (tok.get_message_id() != 0)
        tmpMsg = tmpMsg + " for token: [" + std::to_string(tok.get_message_id()) + "]";

    auto tmpTopic = tok.get_topics();
    if (tmpTopic && !tmpTopic->empty())
        tmpMsg = tmpMsg + ". Token topic: \"" + (*tmpTopic)[0] + "\"";
        
    ATLAS_LOGGER_INFO(tmpMsg);
    done_ = true;
}

} //namespace atlas
