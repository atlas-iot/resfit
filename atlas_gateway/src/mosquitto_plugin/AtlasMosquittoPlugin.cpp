#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <mosquitto.h>
#include <mosquitto_broker.h>
#include <mosquitto_plugin.h>
#include <fnmatch.h>
#include <time.h>
#include "logger/AtlasLogger.h"
#include "filter/AtlasFilter.h"
#include "filter/AtlasPacket.h"

#define ATLAS_MOSQUITTO_PLUGIN_START_MSG "Atlas gateway mosquitto plug-in starting..."

#ifdef __cplusplus
extern "C" {
#endif

int mosquitto_auth_plugin_version(void)
{
    return MOSQ_AUTH_PLUGIN_VERSION;
}

int mosquitto_auth_plugin_init(void **user_data, struct mosquitto_opt *opts, int opt_count)
{
    atlas::atlasLoggerInit();

    atlas::AtlasFilter::getInstance().init();

    ATLAS_LOGGER_DEBUG(ATLAS_MOSQUITTO_PLUGIN_START_MSG);
    
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_plugin_cleanup(void *userdata, struct mosquitto_opt *auth_opts, int auth_opt_count)
{
    atlas::atlasLoggerClose();
    
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_init(void *userdata, struct mosquitto_opt *auth_opts, int auth_opt_count, bool reload)
{
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_security_cleanup(void *userdata, struct mosquitto_opt *auth_opts, int auth_opt_count, bool reload)
{
    return MOSQ_ERR_SUCCESS;
}

int mosquitto_auth_pkt_inspect(void *user_data, const char *src_client_id, const char *src_username,
                               const char *dst_client_id, const char *dst_username, const char *topic,
                               uint8_t qos, uint16_t payload_len, const uint8_t *payload)
{
    atlas::AtlasPacket pkt;

    ATLAS_LOGGER_DEBUG("Inspecting Mosquitto packet: \n"
                            "\tSource client id: %s\n"
                            "\tSource username: %s\n"
                            "\tDestination client id: %s\n"
                            "\tDestination username: %s\n"
                            "\tTopic: %s\n"
                            "\tQoS: %d\n"
                            "\tPayload length: %d",
                            src_client_id,
                            src_username,
                            dst_client_id,
                            dst_username,
                            topic,
                            qos,
                            payload_len);

    pkt.setSrcUsername(src_username);
    pkt.setSrcClientId(src_client_id);
    pkt.setDstUsername(dst_username);
    pkt.setDstClientId(dst_client_id);
    pkt.setTopic(topic);
    pkt.setQos(qos);
    pkt.setPayload(payload);
    pkt.setPayloadLen(payload_len);

    if (atlas::AtlasFilter::getInstance().filter(pkt))
        return MOSQ_ERR_SUCCESS;
    
    return MOSQ_ERR_PKT_DROP;
}


int mosquitto_auth_unpwd_check(void *user_data, struct mosquitto *client, const char *username, const char *password)
{
    return 0;
}

int mosquitto_auth_acl_check(void *user_data, int access, struct mosquitto *client, const struct mosquitto_acl_msg *msg)
{
    return 0;
}

#ifdef __cplusplus
}
#endif
