#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <limits.h>
#include <errno.h>
#include "atlas_telemetry_sysinfo.h"
#include "../logger/atlas_logger.h"
#include "../commands/atlas_command.h"
#include "../commands/atlas_command_types.h"
#include "../identity/atlas_identity.h"
#include "../coap/atlas_coap_server.h"
#include "../utils/atlas_config.h"
#include "atlas_telemetry.h"
#include "atlas_alert_utils.h"

#define ATLAS_SYSINFO_FEATURE_MAX_LEN (32)
#define ATLAS_SYSINFO_UPTIME_PATH "gateway/telemetry/sysinfo/uptime"
#define ATLAS_SYSINFO_TOTALRAM_PATH "gateway/telemetry/sysinfo/totalram"
#define ATLAS_SYSINFO_FREERAM_PATH "gateway/telemetry/sysinfo/freeram"
#define ATLAS_SYSINFO_SHAREDRAM_PATH "gateway/telemetry/sysinfo/sharedram"
#define ATLAS_SYSINFO_BUFFERRAM_PATH "gateway/telemetry/sysinfo/bufferram"
#define ATLAS_SYSINFO_TOTALSWAP_PATH "gateway/telemetry/sysinfo/totalswap"
#define ATLAS_SYSINFO_FREESWAP_PATH "gateway/telemetry/sysinfo/freeswap"
#define ATLAS_SYSINFO_PROCS_PATH "gateway/telemetry/sysinfo/procs"
#define ATLAS_SYSINFO_LOAD1_PATH "gateway/telemetry/sysinfo/load1"
#define ATLAS_SYSINFO_LOAD5_PATH "gateway/telemetry/sysinfo/load5"
#define ATLAS_SYSINFO_LOAD15_PATH "gateway/telemetry/sysinfo/load15"

#define ATLAS_PUSH_ALERT_SYSINFO_PROCS_PATH "client/telemetry/sysinfo/procs/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_UPTIME_PATH "client/telemetry/sysinfo/uptime/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_FREERAM_PATH "client/telemetry/sysinfo/freeram/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_SHAREDRAM_PATH "client/telemetry/sysinfo/sharedram/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_BUFFERRAM_PATH "client/telemetry/sysinfo/bufferram/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_TOTALSWAP_PATH "client/telemetry/sysinfo/totalswap/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_FREESWAP_PATH "client/telemetry/sysinfo/freeswap/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_LOAD1_PATH "client/telemetry/sysinfo/load1/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_LOAD5_PATH "client/telemetry/sysinfo/load5/alerts/push"
#define ATLAS_PUSH_ALERT_SYSINFO_LOAD15_PATH "client/telemetry/sysinfo/load15/alerts/push"

#define ATLAS_THRESHOLD_ALERT_SYSINFO_PROCS_PATH "client/telemetry/sysinfo/procs/alerts/threshold"

static uint16_t procs_threshold;

static void
atlas_telemetry_payload_uptime(uint8_t **payload, uint16_t *payload_len,
                               uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char uptime[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo uptime telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo uptime telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo uptime");
        return;
    }

    sprintf(uptime, "%lu",info.uptime);

    /* Add sysinfo uptime command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_UPTIME, strlen(uptime), (uint8_t *)uptime);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_totalram(uint8_t **payload, uint16_t *payload_len,
                                 uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char totalram[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo totalram telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo totalram telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo totalram");
        return;
    }

    sprintf(totalram, "%lu",info.totalram);

    /* Add sysinfo totalram command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_TOTALRAM, strlen(totalram), (uint8_t *)totalram);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_freeram(uint8_t **payload, uint16_t *payload_len,
                                uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char freeram[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo freeram telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo freeram telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo freeram");
        return;
    }

    sprintf(freeram, "%lu",info.freeram);

    /* Add sysinfo freeram command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_FREERAM, strlen(freeram), (uint8_t *)freeram);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_sharedram(uint8_t **payload, uint16_t *payload_len,
                                  uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char sharedram[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo sharedram telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo sharedram telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo sharedram");
        return;
    }

    sprintf(sharedram, "%lu",info.sharedram);

    /* Add sysinfo sharedram command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_SHAREDRAM, strlen(sharedram), (uint8_t *)sharedram);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_bufferram(uint8_t **payload, uint16_t *payload_len,
                                  uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char bufferram[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo bufferram telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo bufferram telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo bufferram");
        return;
    }

    sprintf(bufferram, "%lu",info.bufferram);

    /* Add sysinfo bufferram command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_BUFFERRAM, strlen(bufferram), (uint8_t *)bufferram);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_totalswap(uint8_t **payload, uint16_t *payload_len,
                                  uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char totalswap[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo totalswap telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo totalswap telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo totalswap");
        return;
    }

    sprintf(totalswap, "%lu",info.totalswap);

    /* Add sysinfo totalswap command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_TOTALSWAP, strlen(totalswap), (uint8_t *)totalswap);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_freeswap(uint8_t **payload, uint16_t *payload_len,
                                 uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char freeswap[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo freeswap telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo freeswap telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo freeswap");
        return;
    }

    sprintf(freeswap, "%lu",info.freeswap);

    /* Add sysinfo uptime command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_FREESWAP, strlen(freeswap), (uint8_t *)freeswap);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_procs(uint8_t **payload, uint16_t *payload_len,
                              uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char procs[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo procs telemetry feature");
    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo procs");
        return;
    }

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD && info.procs < procs_threshold) {
        ATLAS_LOGGER_INFO("Sysinfo freeswap telemetry feature does not support thresholds");
        return;
    }

    sprintf(procs, "%u",info.procs);

    /* Add sysinfo procs command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_PROCS, strlen(procs), (uint8_t *)procs);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_load1(uint8_t **payload, uint16_t *payload_len,
                              uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char load1[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo load1 telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo load1 telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo load1");
        return;
    }

    sprintf(load1, "%lu",info.loads[0]);

    /* Add sysinfo load1 command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_LOAD1, strlen(load1), (uint8_t *)load1);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_load5(uint8_t **payload, uint16_t *payload_len,
                              uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char load5[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo load5 telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo load5 telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo load5");
        return;
    }

    sprintf(load5, "%lu",info.loads[1]);

    /* Add sysinfo load5 command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_LOAD5, strlen(load5), (uint8_t *)load5);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static void
atlas_telemetry_payload_load15(uint8_t **payload, uint16_t *payload_len,
                               uint8_t use_threshold)
{
    atlas_cmd_batch_t *cmd_batch;
    uint8_t *cmd_buf = NULL;
    uint16_t cmd_len = 0;
    const char *identity = atlas_identity_get();
    struct sysinfo info;
    char load15[ATLAS_SYSINFO_FEATURE_MAX_LEN + 1] = { 0 };

    ATLAS_LOGGER_INFO("Get payload for sysinfo load15 telemetry feature");

    if (use_threshold == ATLAS_TELEMETRY_USE_THRESHOLD) {
        ATLAS_LOGGER_ERROR("Sysinfo load15 telemetry feature does not support thresholds");
	return;
    }

    if (sysinfo(&info) != 0) {
        ATLAS_LOGGER_ERROR("Error in getting sysinfo load15");
        return;
    }

    sprintf(load15, "%lu",info.loads[2]);

    /* Add sysinfo load15 command */
    cmd_batch = atlas_cmd_batch_new();
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_IDENTITY, strlen(identity), (uint8_t *)identity);
    atlas_cmd_batch_add(cmd_batch, ATLAS_CMD_TELEMETRY_SYSINFO_LOAD15, strlen(load15), (uint8_t *)load15);

    atlas_cmd_batch_get_buf(cmd_batch, &cmd_buf, &cmd_len);

    *payload = malloc(cmd_len);
    memcpy(*payload, cmd_buf, cmd_len);
    *payload_len = cmd_len;

    atlas_cmd_batch_free(cmd_batch);
}

static atlas_coap_response_t
atlas_push_alert_cb(const char *uri_path, const uint8_t *req_payload, size_t req_payload_len,
                    uint8_t **resp_payload, size_t *resp_payload_len)
{
    atlas_status_t status;
    uint16_t push_rate;

    ATLAS_LOGGER_DEBUG("Telemetry sysinfo push alert end-point called");

    status = atlas_alert_push_cmd_parse(req_payload, req_payload_len, &push_rate);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_DEBUG("Telemetry sysinfo uptime push alert end-point encountered an error when parsing the command");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE_HERE;
    }

    if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_PROCS_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_PROCS_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_UPTIME_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_UPTIME_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_FREERAM_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_FREERAM_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_SHAREDRAM_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_SHAREDRAM_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_BUFFERRAM_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_BUFFERRAM_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_TOTALSWAP_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_TOTALSWAP_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_FREESWAP_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_FREESWAP_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_LOAD1_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_LOAD1_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_LOAD5_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_LOAD5_PATH, push_rate);
    else if (!strcmp(uri_path, ATLAS_PUSH_ALERT_SYSINFO_LOAD15_PATH))
        atlas_telemetry_push_set(ATLAS_SYSINFO_LOAD15_PATH, push_rate);
 
    return ATLAS_COAP_RESP_OK;
}

static atlas_coap_response_t
atlas_threshold_alert_procs_cb(const char *uri_path, const uint8_t *req_payload, size_t req_payload_len,
                               uint8_t **resp_payload, size_t *resp_payload_len)
{
    atlas_status_t status;
    uint16_t scan_rate;
    char *threshold = NULL;

    ATLAS_LOGGER_DEBUG("Telemetry sysinfo threshold alert end-point called");

    status = atlas_alert_threshold_cmd_parse(req_payload, req_payload_len, &scan_rate,
                                             &threshold);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Telemetry sysinfo threshold alert end-point encountered an error when parsing the command");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE_HERE;
    }

    errno = 0;
    procs_threshold = strtol(threshold, NULL, 10);

    free(threshold);
   
    /* Check for various possible errors */
    if ((errno == ERANGE && (procs_threshold == LONG_MAX || procs_threshold == LONG_MIN))
            || (errno != 0 && procs_threshold == 0)) {
        ATLAS_LOGGER_ERROR("Telemetry sysinfo threshold alert end-point encountered an error when parsing the threshold value");
        return ATLAS_COAP_RESP_NOT_ACCEPTABLE_HERE;
    }

    atlas_telemetry_threshold_set(ATLAS_SYSINFO_PROCS_PATH, scan_rate);

    return ATLAS_COAP_RESP_OK;
}

void
atlas_telemetry_add_sysinfo()
{
    atlas_status_t status;

    ATLAS_LOGGER_DEBUG("Add sysinfo telemetry feature");

    /* Add sysinfo telemetry features */
    atlas_telemetry_add(ATLAS_SYSINFO_UPTIME_PATH, atlas_telemetry_payload_uptime);
    atlas_telemetry_add(ATLAS_SYSINFO_TOTALRAM_PATH, atlas_telemetry_payload_totalram);
    atlas_telemetry_add(ATLAS_SYSINFO_FREERAM_PATH, atlas_telemetry_payload_freeram);
    atlas_telemetry_add(ATLAS_SYSINFO_SHAREDRAM_PATH, atlas_telemetry_payload_sharedram);
    atlas_telemetry_add(ATLAS_SYSINFO_BUFFERRAM_PATH, atlas_telemetry_payload_bufferram);
    atlas_telemetry_add(ATLAS_SYSINFO_TOTALSWAP_PATH, atlas_telemetry_payload_totalswap);
    atlas_telemetry_add(ATLAS_SYSINFO_FREESWAP_PATH, atlas_telemetry_payload_freeswap);
    atlas_telemetry_add(ATLAS_SYSINFO_PROCS_PATH, atlas_telemetry_payload_procs);
    atlas_telemetry_add(ATLAS_SYSINFO_LOAD1_PATH, atlas_telemetry_payload_load1);
    atlas_telemetry_add(ATLAS_SYSINFO_LOAD5_PATH, atlas_telemetry_payload_load5);
    atlas_telemetry_add(ATLAS_SYSINFO_LOAD15_PATH, atlas_telemetry_payload_load15);

    /* Add sysinfo telemetry push alerts */
    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_PROCS_PATH, ATLAS_COAP_METHOD_PUT,
                                            atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo procs push telemetry alert end-point");
        return;
    }
    
    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_UPTIME_PATH, ATLAS_COAP_METHOD_PUT,
                                            atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo uptime push telemetry alert end-point");
        return;
    }
   
    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_FREERAM_PATH, ATLAS_COAP_METHOD_PUT,
                                           atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo freeram push telemetry alert end-point");
        return;
    }

    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_SHAREDRAM_PATH, ATLAS_COAP_METHOD_PUT,
                                            atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo sharedram push telemetry alert end-point");
        return;
    }

    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_BUFFERRAM_PATH, ATLAS_COAP_METHOD_PUT,
                                            atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo bufferram push telemetry alert end-point");
        return;
    }

    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_TOTALSWAP_PATH, ATLAS_COAP_METHOD_PUT,
                                            atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo totalswap push telemetry alert end-point");
        return;
    }

    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_FREESWAP_PATH, ATLAS_COAP_METHOD_PUT,
                                           atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo freeswap push telemetry alert end-point");
        return;
    }

    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_LOAD1_PATH, ATLAS_COAP_METHOD_PUT,
                                           atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo load1 push telemetry alert end-point");
        return;
    }

    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_LOAD5_PATH, ATLAS_COAP_METHOD_PUT,
                                           atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo load5 push telemetry alert end-point");
        return;
    }
   
    status = atlas_coap_server_add_resource(ATLAS_PUSH_ALERT_SYSINFO_LOAD15_PATH, ATLAS_COAP_METHOD_PUT,
                                           atlas_push_alert_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo load15 push telemetry alert end-point");
        return;
    }

    /* Add sysinfo telemetry threshold alerts */
    status = atlas_coap_server_add_resource(ATLAS_THRESHOLD_ALERT_SYSINFO_PROCS_PATH, ATLAS_COAP_METHOD_PUT,
                                            atlas_threshold_alert_procs_cb);
    if (status != ATLAS_OK) {
        ATLAS_LOGGER_ERROR("Cannot install sysinfo procs threshold telemetry alert end-point");
        return;
    }
}
