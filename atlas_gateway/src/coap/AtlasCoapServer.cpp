#include <iostream>
#include <coap2/coap.h>
#include <exception>
#include <coap2/coap.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "AtlasCoapServer.h"
#include "AtlasCoapException.h"
#include "../scheduler/AtlasScheduler.h"
#include "../logger/AtlasLogger.h"
#include "../device/AtlasDeviceManager.h"

#define ATLAS_COAP_SERVER_IS_UDP(MODE) ((MODE) & ATLAS_COAP_SERVER_MODE_UDP)
#define ATLAS_COAP_SERVER_IS_DTLS(MODE) ((MODE) & ATLAS_COAP_SERVER_MODE_DTLS_PSK)

namespace atlas {

const std::string DEFAULT_INDEX = "ATLAS CoAP gateway";

void AtlasCoapServer::getHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                                 coap_binary_t *token, coap_string_t *query, coap_pdu_t *response)
{
    ATLAS_LOGGER_DEBUG("Handling CoAP GET request...");

    AtlasCoapServer::getInstance().incomingHandler(ctx, resource, session,
                                                   request, token, query, response,
                                                   ATLAS_COAP_METHOD_GET);
}

void AtlasCoapServer::postHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                                 coap_binary_t *token, coap_string_t *query, coap_pdu_t *response)
{
    ATLAS_LOGGER_DEBUG("Handling CoAP POST request...");

    AtlasCoapServer::getInstance().incomingHandler(ctx, resource, session,
                                                   request, token, query, response,
                                                   ATLAS_COAP_METHOD_POST);
}

void AtlasCoapServer::putHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                                 coap_binary_t *token, coap_string_t *query, coap_pdu_t *response)
{
    ATLAS_LOGGER_DEBUG("Handling CoAP PUT request...");

    AtlasCoapServer::getInstance().incomingHandler(ctx, resource, session,
                                                   request, token, query, response,
                                                   ATLAS_COAP_METHOD_PUT);
}

void AtlasCoapServer::deleteHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                                 coap_binary_t *token, coap_string_t *query, coap_pdu_t *response)
{
    ATLAS_LOGGER_DEBUG("Handling CoAP DELETE request...");

    AtlasCoapServer::getInstance().incomingHandler(ctx, resource, session,
                                                   request, token, query, response,
                                                   ATLAS_COAP_METHOD_DELETE);
}

void AtlasCoapServer::incomingHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                                      coap_binary_t *token, coap_string_t *query, coap_pdu_t *response, AtlasCoapMethod method)
{
    coap_string_t *coapUriPath;
    char *uriPath;
    uint8_t *reqPayload = NULL;
    size_t reqPayloadLen = 0;
    uint8_t *respPayload = NULL;
    size_t respPayloadLen = 0;
    AtlasCoapResource coapResource;
    AtlasCoapResponse respCode;
    std::string identity = "";
    std::string psk = "";

    ATLAS_LOGGER_DEBUG("Handling incoming CoAP request...");
    
    /* Assign CoAP PSK identity if the transport is DTLS */
    if (session->proto == COAP_PROTO_DTLS) {
        ATLAS_LOGGER_DEBUG("Assigning PSK DTLS identity from the transport layer");
        identity.assign((const char*)session->psk_identity->s, session->psk_identity->length);
        psk.assign((const char*)session->psk_key->s, session->psk_key->length);
    }

    /* Get request payload if any */
    coap_get_data(request, &reqPayloadLen, &reqPayload);

    coapUriPath = coap_get_uri_path(request);
    if (!coapUriPath) {
        ATLAS_LOGGER_ERROR("Drop CoAP request: cannot get URI path!");
        return;
    }

    uriPath = new char[coapUriPath->length + 1];
    memset(uriPath, 0, coapUriPath->length + 1);
    memcpy(uriPath, coapUriPath->s, coapUriPath->length);

    coapResource = this->resources_[uriPath];
    if (!coapResource.getCallback()) {
        ATLAS_LOGGER_DEBUG("No callback registered. Dropping request...");
	goto RET;
    }

    respCode = coapResource.getCallback()(uriPath, identity, psk, method, reqPayload, reqPayloadLen, &respPayload, &respPayloadLen);

    response->code = COAP_RESPONSE_CODE(respCode);

    /* Add payload if required */
    if (respCode == ATLAS_COAP_RESP_OK && respPayload && respPayloadLen)
        coap_add_data_blocked_response(resource, session, request, response, token,
                                       COAP_MEDIATYPE_TEXT_PLAIN, 0x2ffff,
                                       respPayloadLen, respPayload);

    ATLAS_LOGGER_DEBUG("CoAP server response is sent");

RET:
    delete[] uriPath;
    delete[] respPayload;
}

AtlasCoapServer& AtlasCoapServer::getInstance()
{
    static AtlasCoapServer instance;

    return instance;
}

AtlasCoapServer::AtlasCoapServer() : ctx_(nullptr) {}

void AtlasCoapServer::schedulerCallback()
{
    coap_io_process(ctx_, COAP_IO_NO_WAIT);
}

void AtlasCoapServer::getDefaultIndexHandler(coap_context_t *ctx,
                                             struct coap_resource_t *resource,
                                             coap_session_t *session,
                                             coap_pdu_t *request,
                                             coap_binary_t *token,
                                             coap_string_t *query,
                                             coap_pdu_t *response)
{

    const char *INDEX = DEFAULT_INDEX.c_str();

    ATLAS_LOGGER_DEBUG("Serving CoAP default index...");

    coap_add_data_blocked_response(resource, session, request, response, token,
                                   COAP_MEDIATYPE_TEXT_PLAIN, 0x2ffff,
                                   strlen(INDEX),
                                   (const uint8_t *)INDEX);
}

void AtlasCoapServer::initDefaultResources(coap_context_t *ctx)
{
    coap_resource_t *resource;

    resource = coap_resource_init(NULL, 0);
    coap_register_handler(resource, COAP_REQUEST_GET, &AtlasCoapServer::getDefaultIndexHandler);
    coap_add_resource(ctx, resource);
}

const coap_bin_const_t *AtlasCoapServer::getPskForIdentity(coap_bin_const_t *identityVal)
{
    std::string identity;
    const char *pskVal;

    identity.assign(reinterpret_cast<const char *> (identityVal->s), identityVal->length);

    AtlasDevice *device = AtlasDeviceManager::getInstance().getDevice(identity);
    if(!device) {
        ATLAS_LOGGER_ERROR("No client device exists in db with identity " + identity);
        return nullptr;
    }
    
    ATLAS_LOGGER_DEBUG("Getting PSK for client...");
    
    pskVal = device->getPskAsCharArray();
    identityPsk_.s = reinterpret_cast<const uint8_t *> (pskVal);
    identityPsk_.length = device->getPsk().length();
    
    return &identityPsk_;
}

const coap_bin_const_t *AtlasCoapServer::verifyIdentity(coap_bin_const_t *identity,
                                                        coap_session_t *c_session,
                                                        void *arg)
{
    ATLAS_LOGGER_DEBUG("Verify PSK identity - choosing the PSK for the client identity");

    return AtlasCoapServer::getInstance().getPskForIdentity(identity);
}

void AtlasCoapServer::setDtlsPsk(coap_context_t *ctx)
{
    coap_dtls_spsk_t psk;

    if (!coap_dtls_is_supported())
        throw AtlasCoapException("CoAP DTLS is not supported");
 
    memset(&psk, 0, sizeof(psk));

    psk.version = COAP_DTLS_SPSK_SETUP_VERSION;

    /* Key will be set when the session is established (based on the client identity) */ 
    psk.psk_info.key.s = nullptr;
    psk.psk_info.key.length = 0;

    /* SNI for virtual host callback */
    psk.validate_id_call_back = &AtlasCoapServer::verifyIdentity;

    /* Set PSK */
    coap_context_set_psk2(ctx, &psk);
 }

coap_context_t* AtlasCoapServer::getContext(uint16_t port, AtlasCoapServerMode mode)
{
    coap_context_t *ctx = NULL;
    coap_address_t listen_addr, listen_addrs;
    uint16_t tmp;
    coap_endpoint_t *ep = NULL;
    coap_endpoint_t *eps = NULL;

    ctx = coap_new_context(NULL);
    if (!ctx)
        return NULL;
 
    /* Set DTLS PSK */
    if (ATLAS_COAP_SERVER_IS_DTLS(mode))
        this->setDtlsPsk(ctx);
 
    coap_address_init(&listen_addr);
    listen_addr.addr.sa.sa_family = AF_INET;
    listen_addr.addr.sin.sin_port = htons (port);
    listen_addrs = listen_addr;

    if (ATLAS_COAP_SERVER_IS_UDP(mode) && ATLAS_COAP_SERVER_IS_DTLS(mode)) {
        if (listen_addr.addr.sa.sa_family == AF_INET) {
            tmp = ntohs(listen_addr.addr.sin.sin_port) + 1;
            listen_addrs.addr.sin.sin_port = htons(tmp);
        } else if (listen_addr.addr.sa.sa_family == AF_INET6) {
            uint16_t temp = ntohs(listen_addr.addr.sin6.sin6_port) + 1;
            listen_addrs.addr.sin6.sin6_port = htons(temp);
        }
    }

    if (ATLAS_COAP_SERVER_IS_UDP(mode)) {
        ep = coap_new_endpoint(ctx, &listen_addr, COAP_PROTO_UDP);
        if (!ep) {
            ATLAS_LOGGER_ERROR("Cannot open COAP UDP");
            goto ERROR;
        }
    }

    if (ATLAS_COAP_SERVER_IS_DTLS(mode)) {
        eps = coap_new_endpoint(ctx, &listen_addrs, COAP_PROTO_DTLS);
        if (!eps) {
            ATLAS_LOGGER_ERROR("Cannot open COAP DTLS");
            goto ERROR;
        }
    }


    return ctx;

ERROR:
    coap_free_endpoint(ep);
    coap_free_endpoint(eps);
    coap_free_context(ctx);

    return NULL;
}

void AtlasCoapServer::start(uint16_t port, AtlasCoapServerMode mode)
{
    int fd;

    if (!port)
        throw AtlasCoapException("Invalid port");

    /* Start CoAP */
    coap_startup();

    /* Get CoAP context */
    ctx_ = this->getContext(port, mode);
    if (!ctx_) {
        ATLAS_LOGGER_DEBUG("Cannot create CoAP context");
        throw AtlasCoapException("Invalid CoAP context");
    }

    /* Init default resources */
    this->initDefaultResources(ctx_);

    fd = coap_context_get_coap_fd(ctx_);
    if (fd == -1) {
        ATLAS_LOGGER_INFO("Cannot get CoAP file descriptor");
        coap_free_context(ctx_);
        ctx_ = NULL;
        throw AtlasCoapException("Cannot get file descriptor for CoAP server");
    }

    /* Schedule CoAP server */
    AtlasScheduler::getInstance().addFdEntry(fd, std::bind(&AtlasCoapServer::schedulerCallback, this));
}

void AtlasCoapServer::addResource(const AtlasCoapResource &resource)
{
    coap_resource_t *coapResource;

    if (!this->ctx_) {
        ATLAS_LOGGER_ERROR("Invalid CoAP context. Probably the server is not started");
	throw AtlasCoapException("Invalid CoAP context");
    }

    coapResource = coap_resource_init(coap_make_str_const(resource.getUri().c_str()), 0);
    if (!coapResource) {
        ATLAS_LOGGER_ERROR("CoAP server: error when creating CoAP resource");
        throw AtlasCoapException("Cannot create CoAP server resource");
    }

    if (resource.getCoapMethod() == ATLAS_COAP_METHOD_GET)
        coap_register_handler(coapResource, COAP_REQUEST_GET, &AtlasCoapServer::getHandler);
    else if (resource.getCoapMethod() == ATLAS_COAP_METHOD_POST)
        coap_register_handler(coapResource, COAP_REQUEST_POST, &AtlasCoapServer::postHandler);
    else if (resource.getCoapMethod() == ATLAS_COAP_METHOD_PUT)
        coap_register_handler(coapResource, COAP_REQUEST_PUT, &AtlasCoapServer::putHandler);
    else if (resource.getCoapMethod() == ATLAS_COAP_METHOD_DELETE)
        coap_register_handler(coapResource, COAP_REQUEST_DELETE, &AtlasCoapServer::deleteHandler);

    coap_add_resource(this->ctx_, coapResource);

    this->resources_[resource.getUri()] = resource;
}

void AtlasCoapServer::delResource(const AtlasCoapResource &resource)
{
    this->resources_.erase(resource.getUri());
}

} // namespace atlas
