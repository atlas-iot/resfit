#ifndef __ATLAS_COMMANDS_CLOUD_H__
#define __ATLAS_COMMANDS_CLOUD_H__

namespace atlas {

/* Publish-subscribe topic suffix which allows full-duplex communication with the cloud */
const std::string ATLAS_TO_CLOUD_TOPIC = "-to-cloud";
const std::string ATLAS_TO_GATEWAY_TOPIC = "-to-gateway";

/* Cloud command structure */
const std::string ATLAS_CMD_TYPE_JSON_KEY = "commandType";
const std::string ATLAS_CMD_PAYLOAD_JSON_KEY = "commandPayload";

/* Cloud command type sent from gateway to cloud */
const std::string ATLAS_CMD_GATEWAY_INFO_UPDATE = "ATLAS_CMD_GATEWAY_INFO_UPDATE";
const std::string ATLAS_CMD_GATEWAY_CLIENT_INFO_UPDATE = "ATLAS_CMD_GATEWAY_CLIENT_INFO_UPDATE";
const std::string ATLAS_CMD_GATEWAY_REGISTER = "ATLAS_CMD_GATEWAY_REGISTER";
const std::string ATLAS_CMD_GATEWAY_KEEPALIVE = "ATLAS_CMD_GATEWAY_KEEPALIVE";
const std::string ATLAS_CMD_GATEWAY_CLIENT_ACK = "ATLAS_CMD_GATEWAY_CLIENT_ACK";
const std::string ATLAS_CMD_GATEWAY_CLIENT_DONE = "ATLAS_CMD_GATEWAY_CLIENT_DONE";

/* Cloud command type send from cloud to gateway */
const std::string ATLAS_CMD_GATEWAY_GET_ALL_DEVICES = "ATLAS_CMD_GATEWAY_GET_ALL_DEVICES";
const std::string ATLAS_CMD_GATEWAY_REGISTER_REQUEST = "ATLAS_CMD_GATEWAY_REGISTER_REQUEST";
const std::string ATLAS_CMD_GATEWAY_CLIENT = "ATLAS_CMD_GATEWAY_CLIENT";
const std::string ATLAS_CMD_GATEWAY_ACK_FOR_DONE_COMMAND = "ATLAS_CMD_GATEWAY_CLIENT_DONE_ACK";
} // namespace atlas


#endif /* __ATLAS_COMMANDS_CLOUD_H__ */
