#ifndef __ATLAS_CLOUD_CMD_PARSER_H__
#define __ATLAS_CLOUD_CMD_PARSER_H__

#include "../alarm/AtlasAlarm.h"
#include "../mqtt_client/IAtlasMqttState.h"
#include "../sql/AtlasSQLite.h"
#include <boost/function.hpp>
#include <json/json.h>
#include <unordered_map>

namespace atlas {

class AtlasCloudCmdParser: public IAtlasMqttState
{
public:
    /**
    * @brief Get instance for cloud control module
    * @return Cloud control module instance
    */
    static AtlasCloudCmdParser& getInstance();

    /**
    * @brief MQTT broker connect callback
    * @return none
    */
    void onConnect() override;

    /**
    * @brief MQTT broker disconnect callback
    * @return none
    */
    void onDisconnect() override;

    /**
    * @brief Start cloud command parser module
    * @return none
    */
    void start();

    /**
    * @brief Stop cloud command parser module
    * @return none
    */
    void stop();

    /**
    * @brief Parse a received command
    * @param[in] command
    * @return none
    */
    void parseCmd(const std::string &cmd);

    AtlasCloudCmdParser(const AtlasCloudCmdParser &) = delete;
    AtlasCloudCmdParser& operator=(const AtlasCloudCmdParser &) = delete;

private:
    /**
    * @brief Default ctor for cloud control module
    * @return none
    */
    AtlasCloudCmdParser();

    /**
    * @brief Callback for ATLAS_CMD_GET_ALL_DEVICES command
    * @return none
    */
    void getAllDevicesCmd();

    /**
    * @brief Callback for ATLAS_CMD_GATEWAY_REGISTER_REQUEST command
    * @return none
    */
    void reqRegisterCmd();

    /**
    * @brief Callback for ATLAS_CMD_GATEWAY_CLIENT command
    * @param[in] cmdPayload Command payload
    * @return none
    */
    void deviceApprovedCmd(const Json::Value &cmdPayload);

    /**
    * @brief Callback for ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND command
    * @param[in] cmdPayload Command payload
    * @return none
    */
    void rcvACKForDONEDeviceCommand(const Json::Value &cmdPayload);

    /* Indicates if the cloud module is connected */
    bool connected_;
};

} // namespace atlas

#endif /* __ATLAS_CLOUD_CMD_PARSER_H__ */
