#include <stdlib.h>
#include "atlas_command_execute_types.h"
#include "atlas_command_execute.h"
#include "atlas_command_execute_utils.h"
#include "../logger/atlas_logger.h"
#include "../coap/atlas_coap_server.h"

#define ATLAS_COMMAND_EXECUTE_PATH "client/approved/command/push"


static atlas_coap_response_t
atlas_execute_command_cb(const char *uri_path, const uint8_t *req_payload, size_t req_payload_len,
                    uint8_t **resp_payload, size_t *resp_payload_len)
{
    atlas_status_t status;

    ATLAS_LOGGER_DEBUG("ATLAS_COMMAND_EXECUTE: Command execution alert end-point called");

    status = atlas_alert_command_execution_parse(req_payload, req_payload_len);
    if (status != ATLAS_OK) {
        if (status == ATLAS_DEFERRED_COMMAND_EXECUTION) {
            ATLAS_LOGGER_DEBUG("ATLAS_COMMAND_EXECUTE: Command execution alert end-point deferred an execution command");
            
            //If needed, a new CoAP response message could be created for this action
            return ATLAS_COAP_RESP_UNKNOWN;     
        }
        ATLAS_LOGGER_DEBUG("ATLAS_COMMAND_EXECUTE: Command execution alert end-point encountered an error when parsing the command");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE_HERE;
    }
 
    return ATLAS_COAP_RESP_OK;
}

bool
atlas_command_execute_init()
{
    ATLAS_LOGGER_DEBUG("ATLAS_COMMAND_EXECUTE: Init command execution engine...");

    atlas_status_t status;

    status = atlas_coap_server_add_resource(ATLAS_COMMAND_EXECUTE_PATH, ATLAS_COAP_METHOD_PUT, atlas_execute_command_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("ATLAS_COMMAND_EXECUTE: Cannot install command execution alert end-point");
        return false;
    }

    return true;
}

