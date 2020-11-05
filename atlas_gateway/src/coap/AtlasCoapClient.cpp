#include "AtlasCoapClient.h"
#include "AtlasCoapException.h"
#include "../logger/AtlasLogger.h"
#include "../scheduler/AtlasScheduler.h"
#include <boost/bind.hpp>

#define ATLAS_COAP_CLIENT_TMP_BUF_LEN (128)

namespace atlas {

AtlasCoapClient &AtlasCoapClient::getInstance()
{
    static AtlasCoapClient instance;

    return instance;
}

uint32_t AtlasCoapClient::getToken()
{
    return token_++;
}

int AtlasCoapClient::resolveAddress(coap_str_const_t *hostname, struct sockaddr *dst)
{
    struct addrinfo *res, *ainfo;
    struct addrinfo hints;
    char *addr;
    int status;
    int len;
 
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_UNSPEC;
 
    addr = new char[hostname->length + 1];
    memset(addr, 0, hostname->length + 1);
    memcpy(addr, hostname->s, hostname->length);
 
    status = getaddrinfo(addr, NULL, &hints, &res);
 
    delete[] addr;
 
    if (status) {
        ATLAS_LOGGER_ERROR("Cannot resolve CoAP request address");
        return -1;
    }
 
    for (ainfo = res; ainfo; ainfo = ainfo->ai_next) {
        if (ainfo->ai_family == AF_INET || ainfo->ai_family == AF_INET6) {
            memcpy(dst, ainfo->ai_addr, ainfo->ai_addrlen);
            len = ainfo->ai_addrlen;
            break;
        }
    }
 
    freeaddrinfo(res);
 
    return len;
}


coap_session_t* AtlasCoapClient::getSession(coap_context_t *ctx, coap_proto_t proto, coap_address_t *dst)
{
    if (proto == COAP_PROTO_DTLS) {
        ATLAS_LOGGER_DEBUG("Create CoAP DTLS session");
        return coap_new_client_session_psk2(ctx, NULL, dst, proto, &dtlsPsk_);
    }

    return coap_new_client_session(ctx, NULL, dst, proto);
}



int AtlasCoapClient::eventHandler(coap_context_t *ctx, coap_event_t event,
                                  struct coap_session_t *session)
{
    ATLAS_LOGGER_INFO("CoAP client event handler");

    switch(event) {
        case COAP_EVENT_DTLS_CLOSED:
            ATLAS_LOGGER_INFO("CoAP client request event: DTLS CLOSED");
            break;
        case COAP_EVENT_TCP_CLOSED:
            ATLAS_LOGGER_INFO("CoAP client request event: TCP CLOSED");
            break;
        case COAP_EVENT_SESSION_CLOSED:
            ATLAS_LOGGER_INFO("CoAP client request event: SESSION CLOSED");
            break;
        default:
            break;
    }

     return 0;
}

void AtlasCoapClient::nackHandler(coap_context_t *context, coap_session_t *session,
                                  coap_pdu_t *sent, coap_nack_reason_t reason,
                                  const coap_tid_t id) {

    ATLAS_LOGGER_INFO("CoAP client nack handler");

    switch(reason) {
        case COAP_NACK_TOO_MANY_RETRIES:
            ATLAS_LOGGER_INFO("Coap client NACK: TOO MANY RETRIES");
            break;
        case COAP_NACK_NOT_DELIVERABLE:
            ATLAS_LOGGER_INFO("Coap client NACK: NOT DELIVERABLE");
            break;
        case COAP_NACK_RST:
            ATLAS_LOGGER_INFO("Coap client NACK: RST");
            break;
        case COAP_NACK_TLS_FAILED:
            ATLAS_LOGGER_INFO("Coap client NACK: TLS FAILED");
            break;
        case COAP_NACK_ICMP_ISSUE:
            ATLAS_LOGGER_INFO("Coap client NACK: ICMP ISSUE");
            break;
        default:
             break;
    }
}

bool AtlasCoapClient::validateToken(coap_pdu_t *received, uint32_t token)
{
    if (received->token_length != sizeof(token))
        return false;

    if (memcmp(received->token, (uint8_t*) &token, sizeof(token)) != 0)
        return false;

    return true;
}


void AtlasCoapClient::messageProcess(struct coap_context_t *ctx,
                                     coap_session_t *session,
                                     coap_pdu_t *sent,
                                     coap_pdu_t *received,
                                     const coap_tid_t id)
{
    uint8_t *respPayload = NULL;
    size_t respPayloadLen = 0;

    ATLAS_LOGGER_DEBUG("Process CoAP response");

    AtlasCoapRequest &req = requests_[ctx];
    if (req.getContext() != ctx || req.getSession() != session) {
        ATLAS_LOGGER_DEBUG("Invalid CoAP context or session. Dropping the CoAP response");
        return;
    }

    /* Validate token */
    if (!validateToken(received, req.getToken())) {
        ATLAS_LOGGER_INFO("Invalid CoAP client response token");
	
        if ((received->type == COAP_MESSAGE_CON || received->type == COAP_MESSAGE_NON) &&
            !sent)
            coap_send_rst(session, received);
	
	return;
    }

    if (received->type == COAP_MESSAGE_RST) {
        ATLAS_LOGGER_INFO("CoAP client: got RST as response");
        req.getCallback()(ATLAS_COAP_RESP_RESET, NULL, 0);
    } else if (COAP_RESPONSE_CLASS(received->code) == 2) {
        ATLAS_LOGGER_DEBUG("CoAP client: Response code is 200");
        
        /* Get CoAP payload, if any */
        coap_get_data(received, &respPayloadLen, &respPayload);
        
        /* Call the higher layer application callback */
        req.getCallback()(ATLAS_COAP_RESP_OK, respPayload, respPayloadLen);
    } else if (COAP_RESPONSE_CLASS(received->code) == 4) {
        ATLAS_LOGGER_DEBUG("CoAP client: Response code is 4XX");
        req.getCallback()(ATLAS_COAP_RESP_NOT_FOUND, NULL, 0);
    } else {
        ATLAS_LOGGER_DEBUG("CoAP client: Response code is UNKNOWN");
        req.getCallback()(ATLAS_COAP_RESP_UNKNOWN, NULL, 0);
    }

    /* If callback is executed, then delete the requests and cancel the timeout, otherwise it will be handled by the timeout */
    requests_.erase(ctx);
    timeouts_.erase(ctx);
}

void AtlasCoapClient::messageHandler(struct coap_context_t *ctx,
                                     coap_session_t *session,
                                     coap_pdu_t *sent,
                                     coap_pdu_t *received,
                                     const coap_tid_t id)
{
    ATLAS_LOGGER_DEBUG("Message handler for CoAP response");

    AtlasCoapClient::getInstance().messageProcess(ctx, session, sent,
                                                  received, id);
}

void AtlasCoapClient::scheduleCallback(coap_context_t *ctx, int fd)
{
    ATLAS_LOGGER_DEBUG("CoAP request scheduler called");

    coap_io_process(ctx, COAP_IO_NO_WAIT);

    /* Cancel scheduling entry if the requests does not exist anymore */
    if (requests_[ctx].getContext() != ctx)
        AtlasScheduler::getInstance().delFdEntry(fd);
}

void AtlasCoapClient::alarmCallback(coap_context_t *ctx, int coapFd)
{
    ATLAS_LOGGER_INFO("Timeout callback executed");

    AtlasCoapRequest &req = requests_[ctx];
   
    if (req.getContext() == ctx) {
        ATLAS_LOGGER_DEBUG("Calling CoAP client callback with TIMEOUT status");
        req.getCallback()(ATLAS_COAP_RESP_TIMEOUT, NULL, 0);
        /* Cancel scheduler for client CoAP file descriptor */
        AtlasScheduler::getInstance().delFdEntry(coapFd); 
        requests_.erase(ctx);
    }

    timeouts_.erase(ctx);
}

void AtlasCoapClient::addRequest(coap_context_t *ctx, coap_session_t *session,
                                 uint32_t token, uint32_t timeout,
                                 coap_request_callback_t callback)
{
    int fd;

    fd = coap_context_get_coap_fd(ctx);
    if (fd < 0)
        throw "Cannot get CoAP file descriptor";

    /* Set alarm for the request */
    timeouts_[ctx] = std::unique_ptr<AtlasAlarm>(new AtlasAlarm("AtlasCoapClient", timeout, true,
                                                 boost::bind(&AtlasCoapClient::alarmCallback, this, ctx, fd)));
    timeouts_[ctx]->start();

    /* Add CoAP request file descriptor to the scheduler */
    requests_[ctx] = AtlasCoapRequest(ctx, session, token, callback);
    AtlasScheduler::getInstance().addFdEntry(fd,
                                             boost::bind(&AtlasCoapClient::scheduleCallback, this, ctx, fd));

    ATLAS_LOGGER_DEBUG("CoAP client request was added");
}

void* AtlasCoapClient::sendRequest(const std::string &uri, AtlasCoapMethod method, const uint8_t *reqPayload,
                                  size_t reqPayloadLen, uint32_t timeout, coap_request_callback_t callback)
{
    coap_uri_t coapUri;
    coap_str_const_t hostname;
    coap_address_t dst;
    coap_context_t *ctx = NULL;
    coap_session_t *session = NULL;
    coap_pdu_t *reqPdu = NULL;
    coap_optlist_t *options = NULL;
    int res;
    uint32_t token;
    uint8_t buf[ATLAS_COAP_CLIENT_TMP_BUF_LEN];
    uint8_t *bufTmp;
    size_t buflen;

    ATLAS_LOGGER_DEBUG("Send CoAP request");
    
    try {

        if (coap_split_uri((uint8_t *)uri.c_str(), uri.length(), &coapUri) < 0)
            throw "Error encountered when parsing CoAP URI";

        /* Resolve CoAP hostname */
        hostname = coapUri.host;
        res = resolveAddress(&hostname, &dst.addr.sa);
        if (res < 0)
            throw "Cannot resolve CoAP address";

        /* Context init */
        ctx = coap_new_context(NULL);
        if (!ctx)
            throw "Cannot create CoAP context for client request";

        /* Register handlers */
	    coap_register_response_handler(ctx, &AtlasCoapClient::messageHandler);
        coap_register_event_handler(ctx, &AtlasCoapClient::eventHandler);
        coap_register_nack_handler(ctx, &AtlasCoapClient::nackHandler);

        dst.size = res;
        dst.addr.sin.sin_port = htons(coapUri.port);

        /* Get session */
        if (coapUri.scheme == COAP_URI_SCHEME_COAPS)
            session = getSession(ctx, COAP_PROTO_DTLS, &dst);
        else
            session = getSession(ctx, COAP_PROTO_UDP, &dst);

        if (!session)
            throw "Cannot create CoAP session for client request";

        /* Create request PDU */
        if (!(reqPdu = coap_new_pdu(session)))
            throw "Cannot create client request PDU";
 
        reqPdu->type = COAP_MESSAGE_CON;
        reqPdu->tid = coap_new_message_id(session);

        switch(method) {
            case ATLAS_COAP_METHOD_GET:
                reqPdu->code = COAP_REQUEST_GET;
                break;

            case ATLAS_COAP_METHOD_POST:
                reqPdu->code = COAP_REQUEST_POST;
                break;

            case ATLAS_COAP_METHOD_PUT:
                reqPdu->code = COAP_REQUEST_PUT;
                break;

            case ATLAS_COAP_METHOD_DELETE:
                reqPdu->code = COAP_REQUEST_DELETE;
                break;

            default:
                reqPdu->code = COAP_REQUEST_GET;
        }

        /* Add token */
        token = getToken();
        if (!coap_add_token(reqPdu, sizeof(uint32_t), (uint8_t*) &token))
            throw "Cannot add token to CoAP client request";

        /* Add URI path */
        if (coapUri.path.length) {
            buflen = sizeof(buf);
            bufTmp = buf;
            res = coap_split_path(coapUri.path.s, coapUri.path.length, bufTmp, &buflen);

            while (res--) {
                coap_insert_optlist(&options,
                                    coap_new_optlist(COAP_OPTION_URI_PATH,
                                    coap_opt_length(bufTmp),
                                    coap_opt_value(bufTmp)));

                bufTmp += coap_opt_size(bufTmp);
            }
        }

        /* Add URI query */
        if (coapUri.query.length) {
            buflen = sizeof(buf);
            bufTmp = buf;
            res = coap_split_query(coapUri.query.s, coapUri.query.length, bufTmp, &buflen);

            while (res--) {
                coap_insert_optlist(&options,
                                    coap_new_optlist(COAP_OPTION_URI_QUERY,
                                    coap_opt_length(buf),
                                    coap_opt_value(buf)));

                bufTmp += coap_opt_size(bufTmp);
            }
        }

        if (options) {
            coap_add_optlist_pdu(reqPdu, &options);
            coap_delete_optlist(options);
        }

        /* Add request payload */
        if (reqPayload && reqPayloadLen)
            coap_add_data(reqPdu, reqPayloadLen, reqPayload);

        /* Add request */
        addRequest(ctx, session, token, timeout, callback); 

        /* Send request */
        ATLAS_LOGGER_DEBUG("Sending CoAP client request...");
        coap_send(session, reqPdu);
        return static_cast<void *>(ctx);

    } catch(const char *e) {
        ATLAS_LOGGER_ERROR(e);

        coap_session_release(session);
        coap_free_context(ctx);

        throw AtlasCoapException(e);
    }
}

void AtlasCoapClient::cancelRequest(void *context)
{
    ATLAS_LOGGER_DEBUG("Canceling CoAP client request...");
    coap_context_t *ctx = static_cast<coap_context_t *>(context);
    if(ctx) {
        requests_.erase(ctx);
        timeouts_.erase(ctx);
    }
}

void
AtlasCoapClient::setDtlsInfo(const std::string &identity, const std::string &psk)
{
    memset(&dtlsPsk_, 0, sizeof(dtlsPsk_));

    dtlsPsk_.version = COAP_DTLS_CPSK_SETUP_VERSION;
     
    /* Set DTLS identity */
    dtlsIdentity_ = identity;
    dtlsPsk_.psk_info.identity.s = (uint8_t *) dtlsIdentity_.c_str();
    dtlsPsk_.psk_info.identity.length = dtlsIdentity_.length();

    /* Set DTLS key */
    dtlsKey_ = psk;
    dtlsPsk_.psk_info.key.s = (uint8_t *) dtlsKey_.c_str();
    dtlsPsk_.psk_info.key.length = dtlsKey_.length();
}

} // namespace atlas

