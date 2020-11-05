#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#include "../scheduler/atlas_scheduler.h"
#include "../logger/atlas_logger.h"
#include "../commands/atlas_command.h"
#include "../commands/atlas_command_types.h"
#include "../utils/atlas_config.h"
#include "../coap/atlas_coap_client.h"
#include "../coap/atlas_coap_response.h"
#include "../alarm/atlas_alarm.h"
#include "../utils/atlas_utils.h"
#include "../identity/atlas_identity.h"
#include "atlas_data_plane_connector.h"

#define ATLAS_CLIENT_DATA_PLANE_BUFFER_LEN (2048)
#define ATLAS_CLIENT_POLICY_TIMEOUT_MS  (5000)
#define ATLAS_CLIENT_FEATURE_TIMEOUT_MS  (5000)
#define ATLAS_CLIENT_POLICY_COAP_PATH   "gateway/policy"
#define ATLAS_CLIENT_FEATURE_COAP_PATH   "gateway/reputation/feature"
#define ATLAS_CLIENT_FEEDBACK_COAP_PATH   "gateway/reputation/feedback"
#define ATLAS_CLIENT_POLICY_ACTIVE (1)

static int connected_socket = -1;
static uint16_t policy_qos;
static uint16_t policy_packets_per_min;
static uint16_t policy_packets_maxlen;
static uint8_t active_policy;
static uint16_t packets_per_min;
static uint16_t packets_avg;

static void set_policy_qos(const uint8_t* qos)
{
    memcpy(&policy_qos, qos, sizeof(policy_qos));
}

static void set_policy_packets_per_min(const uint8_t* ppm)
{
    memcpy(&policy_packets_per_min, ppm, sizeof(policy_packets_per_min));
}

static void set_policy_packets_maxlen(const uint8_t* pack_maxlen)
{
    memcpy(&policy_packets_maxlen, pack_maxlen, sizeof(policy_packets_maxlen));
}

static void set_packets_per_min(const uint8_t* ppm) {
    memcpy(&packets_per_min, ppm, sizeof(packets_per_min));
}

static void set_packets_avg(const uint8_t* pack_avg) {
    memcpy(&packets_avg, pack_avg, sizeof(packets_avg));
}

uint16_t get_packets_per_min() {
    return packets_per_min;
}

uint16_t get_packets_avg() {
    return packets_avg;
}

static void
policy_alarm_callback()
{
    ATLAS_LOGGER_INFO("Policy alarm callback");
    send_policy_command();
}

static void 
policy_callback(const char *uri, atlas_coap_response_t resp_status,
         const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_DEBUG("Policy callback executed");

    if (resp_status != ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_ERROR("Error in sending the policy values");
        
        /* Start sending policy timer */
        if (atlas_alarm_set(ATLAS_CLIENT_POLICY_TIMEOUT_MS, policy_alarm_callback, ATLAS_ALARM_RUN_ONCE) < 0)
            ATLAS_LOGGER_ERROR("Error in scheduling a sending policy alarm!");
        
        return;
    }

    ATLAS_LOGGER_INFO("Sending policy to gateway is COMPLETED!");
}

void
send_policy_command()
{
    atlas_cmd_batch_t *cmd_batch;
    atlas_status_t status;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    uint16_t tmp;
    char uri[ATLAS_URI_MAX_LEN] = { 0 };

    if (active_policy != ATLAS_CLIENT_POLICY_ACTIVE) {
        ATLAS_LOGGER_ERROR("Cannot send empty firewall policy to gateway");
        return;
    }

    cmd_batch = atlas_cmd_batch_new();
    
     /* Add policy qos value */
    tmp = htons(policy_qos);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_POLICY_QOS,
                        sizeof(policy_qos), (uint8_t *) &tmp);

    /* Add policy packets per minute value */
    tmp = htons(policy_packets_per_min);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_PER_MINUTE,
                        sizeof(policy_packets_per_min), (uint8_t *) &tmp);

    /* Add policy packets maxlen value */
    tmp = htons(policy_packets_maxlen);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_MAXLEN,
                        sizeof(policy_packets_maxlen), (uint8_t *) &tmp);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    atlas_cfg_coap_get_uri(ATLAS_CLIENT_POLICY_COAP_PATH, uri);
    status = atlas_coap_client_request(uri, ATLAS_COAP_METHOD_PUT,
                                       cmd_buf, cmd_len, ATLAS_CLIENT_POLICY_TIMEOUT_MS,
                                       policy_callback);


    if (status != ATLAS_OK)
        ATLAS_LOGGER_ERROR("Error when sending policy request");

    atlas_cmd_batch_free(cmd_batch);
}

static atlas_status_t
atlas_reputation_resp_parse(const uint8_t *buf, uint16_t buf_len)
{
    atlas_cmd_batch_t *cmd_batch, *cmd_batch_send;
    const atlas_cmd_t *cmd;
    atlas_status_t status = ATLAS_OK;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;

    if (!buf || !buf_len)
        return ATLAS_INVALID_INPUT;

    cmd_batch = atlas_cmd_batch_new();
    
    status = atlas_cmd_batch_set_raw(cmd_batch, buf, buf_len);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Corrupted feature reputation command");
        status = ATLAS_CORRUPTED_COMMAND;
        goto EXIT;
    }
    
    cmd = atlas_cmd_batch_get(cmd_batch, NULL);
    while (cmd) {
        if (cmd->type == ATLAS_CMD_DATA_PLANE_FEATURE_REPUTATION) {
            cmd_batch_send = atlas_cmd_batch_new();
            /* Add feature reputation value */
            atlas_cmd_batch_add(cmd_batch_send, ATLAS_CMD_DATA_PLANE_FEATURE_REPUTATION, cmd->length,
                                cmd->value);
            atlas_cmd_batch_get_buf(cmd_batch_send, &cmd_buf, &cmd_len);

            /* Send reputation value to data plane */            
            int ret = write(connected_socket, cmd_buf, cmd_len);
            if (ret != cmd_len)
                ATLAS_LOGGER_ERROR("Error writing to socket the reputation value.");
            
            atlas_cmd_batch_free(cmd_batch_send);
        }

        cmd = atlas_cmd_batch_get(cmd_batch, cmd);
    }
     
EXIT:
    atlas_cmd_batch_free(cmd_batch);

    return status;
}

static void 
feature_reputation_callback(const char *uri, atlas_coap_response_t resp_status,
                            const uint8_t *resp_payload, size_t resp_payload_len)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;

    ATLAS_LOGGER_DEBUG("Feature reputation callback executed");

    if (resp_status != ATLAS_COAP_RESP_OK) {
        ATLAS_LOGGER_ERROR("Error in sending the feature request");
        
        /* Send feature reputation error to data plane */
        cmd_batch = atlas_cmd_batch_new();

        /* Add error command */
        atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_FEATURE_ERROR, 0, NULL);
        atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

        /* Send error to data plane */            
        if (write(connected_socket, cmd_buf, cmd_len) != cmd_len) 
            ATLAS_LOGGER_ERROR("Error writing to socket the reputation value.");
        
        atlas_cmd_batch_free(cmd_batch);

        return;
    }

    atlas_reputation_resp_parse(resp_payload, resp_payload_len);
    
    ATLAS_LOGGER_INFO("Sending feature reputation to data plane is COMPLETED!");
}

static void
atlas_feature_reputation_handle(const uint8_t *feature, uint16_t length)
{
    atlas_cmd_batch_t *cmd_batch;
    atlas_status_t status;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    char uri[ATLAS_URI_MAX_LEN] = { 0 };

    if (!feature || !length) {
        ATLAS_LOGGER_ERROR("Invalid feature reputation request");
        return;
    }

    cmd_batch = atlas_cmd_batch_new();

    /* Add identity */
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    
    /* Add feature */
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_FEATURE_REPUTATION, length, feature);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    atlas_cfg_coap_get_uri(ATLAS_CLIENT_FEATURE_COAP_PATH, uri);
    status = atlas_coap_client_request(uri, ATLAS_COAP_METHOD_PUT,
                                       cmd_buf, cmd_len, ATLAS_CLIENT_FEATURE_TIMEOUT_MS,
                                       feature_reputation_callback);

    if (status != ATLAS_OK)
        ATLAS_LOGGER_ERROR("Error when sending feature request");

    atlas_cmd_batch_free(cmd_batch);
}

static void 
feedback_callback(const char *uri, atlas_coap_response_t resp_status,
                            const uint8_t *resp_payload, size_t resp_payload_len)
{
    ATLAS_LOGGER_DEBUG("Feedback callback executed");

    if (resp_status != ATLAS_COAP_RESP_OK)
        ATLAS_LOGGER_ERROR("Error in sending the feedback");
    else
        ATLAS_LOGGER_INFO("Sending feedback to gateway is COMPLETED!");
}

static void
atlas_feature_feedback_handle(const uint8_t *feedback, uint16_t length)
{
    atlas_cmd_batch_t *cmd_batch;
    atlas_status_t status;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    char uri[ATLAS_URI_MAX_LEN] = { 0 };

    if (!feedback || !length) {
        ATLAS_LOGGER_ERROR("Invalid feedback request");
        return;
    }

    cmd_batch = atlas_cmd_batch_new();

    /* Add identity */
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);

    /* Add feedback */
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_FEEDBACK, length, feedback);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    atlas_cfg_coap_get_uri(ATLAS_CLIENT_FEEDBACK_COAP_PATH, uri);
    status = atlas_coap_client_request(uri, ATLAS_COAP_METHOD_PUT,
                                       cmd_buf, cmd_len, ATLAS_CLIENT_FEATURE_TIMEOUT_MS,
                                       feedback_callback);

    if (status != ATLAS_OK)
        ATLAS_LOGGER_ERROR("Error when sending feedback");

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_data_plane_parse_policy(const uint8_t *buf, uint16_t buf_len)
{
    atlas_cmd_batch_t *cmd_batch;
    const atlas_cmd_t *cmd;
    atlas_status_t status;

    if (!buf || !buf_len) {
        ATLAS_LOGGER_ERROR("Corrupted policy command from data plane");
        return;	
    }

    cmd_batch = atlas_cmd_batch_new();

    status = atlas_cmd_batch_set_raw(cmd_batch, buf, buf_len);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Corrupted policy command from data plane");
        atlas_cmd_batch_free(cmd_batch);
        return;
    }

    cmd = atlas_cmd_batch_get(cmd_batch, NULL);
    while (cmd) {
        if (cmd->type == ATLAS_CMD_DATA_PLANE_POLICY_QOS )
            set_policy_qos(cmd->value);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_PER_MINUTE )
            set_policy_packets_per_min(cmd->value);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_POLICY_PACKETS_MAXLEN )
            set_policy_packets_maxlen(cmd->value);

        cmd = atlas_cmd_batch_get(cmd_batch, cmd);
    }

    atlas_cmd_batch_free(cmd_batch);

    active_policy = ATLAS_CLIENT_POLICY_ACTIVE;
    send_policy_command();
}

static void
atlas_data_plane_identity()
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    int ret;

    ATLAS_LOGGER_INFO("Send identity to data plane");

    cmd_batch = atlas_cmd_batch_new();
    
    /* Add identity */
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_DATA_PLANE_IDENTITY, strlen(identity),
                        (const uint8_t *)identity);
    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);
             
    /* Send identity to data plane */
    ret = write(connected_socket, cmd_buf, cmd_len);
    if (ret != cmd_len)
        ATLAS_LOGGER_ERROR("Error writing to socket the reputation value.");
    
    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_data_plane_read_cb(int fd)
{
    uint8_t buf[ATLAS_CLIENT_DATA_PLANE_BUFFER_LEN];
    atlas_cmd_batch_t *cmd_batch;
    const atlas_cmd_t *cmd;
    atlas_status_t status;
    int rc;

    rc = read(fd, buf, sizeof(buf));
    if (rc <= 0) {
        ATLAS_LOGGER_ERROR("Socket read error. Remove socket from scheduler");
        atlas_sched_del_entry(fd);
        return;
    }
    
    cmd_batch = atlas_cmd_batch_new();
    
    status = atlas_cmd_batch_set_raw(cmd_batch, buf, rc);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Corrupted command from data plane");
        atlas_cmd_batch_free(cmd_batch);
        return;
    }

    cmd = atlas_cmd_batch_get(cmd_batch, NULL);
    while (cmd) {
        if (cmd->type == ATLAS_CMD_DATA_PLANE_POLICY)
            atlas_data_plane_parse_policy(cmd->value, cmd->length);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_PACKETS_PER_MINUTE )
            set_packets_per_min(cmd->value);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_PACKETS_AVG)
            set_packets_avg(cmd->value);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_FEATURE_REPUTATION)
            atlas_feature_reputation_handle(cmd->value, cmd->length);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_FEEDBACK)
            atlas_feature_feedback_handle(cmd->value, cmd->length);
        else if (cmd->type == ATLAS_CMD_DATA_PLANE_GET_IDENTITY)
            atlas_data_plane_identity();

        cmd = atlas_cmd_batch_get(cmd_batch, cmd);
    }

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_data_plane_accept_cb(int fd)
{ 
    /* Allow only one connection */
    if (connected_socket != -1) {
        ATLAS_LOGGER_DEBUG("Close existing unix socket...");
        atlas_sched_del_entry(connected_socket);	
	close(connected_socket);
    }

    connected_socket = accept(fd, NULL, NULL);
    if (connected_socket == -1) {
        ATLAS_LOGGER_ERROR("Socket accept error");
	return;
    }

    atlas_sched_add_entry(connected_socket, atlas_data_plane_read_cb);
} 

atlas_status_t
atlas_data_plane_connector_start()
{
    struct sockaddr_un addr;
    int fd;
    int rc;
     
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, ATLAS_DATA_PLANE_UNIX_SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(addr.sun_path);
    
    fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (fd == -1) {
        ATLAS_LOGGER_ERROR("Socket error");
        return ATLAS_SOCKET_ERROR;
    }

    rc = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    if (rc == -1) {
        ATLAS_LOGGER_ERROR("Socket bind error");
        return ATLAS_SOCKET_ERROR;
    }

    rc = listen(fd, 1);
    if (rc == -1) {
        ATLAS_LOGGER_ERROR("Socket listen error");
        return ATLAS_SOCKET_ERROR;
    }
    
    atlas_sched_add_entry(fd, atlas_data_plane_accept_cb);

    return ATLAS_OK;
}

