#ifndef __ATLAS_STATUS_H__
#define __ATLAS_STATUS_H__

typedef enum _atlas_status {
    ATLAS_OK = 0,
    ATLAS_GENERAL_ERR,
    ATLAS_INVALID_HOSTNAME,
    ATLAS_INVALID_PORT,
    ATLAS_COAP_SRV_DTLS_NOT_SUPPORTED,
    ATLAS_COAP_INVALID_URI,
    ATLAS_INVALID_CALLBACK,
    ATLAS_INVALID_DTLS_IDENTITY,
    ATLAS_INVALID_DTLS_PSK,
    ATLAS_IDENTITY_FILE_ERROR,
    ATLAS_PSK_FILE_ERROR,
    ATLAS_INVALID_INPUT,
    ATLAS_CORRUPTED_COMMAND,
    ATLAS_SOCKET_ERROR,
    ATLAS_CORRUPTED_COMMAND_EXECUTION_PAYLOAD,
    ATLAS_DEFERRED_COMMAND_EXECUTION,
} atlas_status_t ;

#endif /* __ATLAS_STATUS_H__ */
