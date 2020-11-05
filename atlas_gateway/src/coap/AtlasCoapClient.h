#ifndef __ATLAS_COAP_CLIENT_H__
#define __ATLAS_COAP_CLIENT_H__

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <string>
#include <coap2/coap.h>
#include <functional>
#include <unordered_map>
#include "AtlasCoapMethod.h"
#include "AtlasCoapResponse.h"
#include "AtlasCoapRequest.h"
#include "../alarm/AtlasAlarm.h"

namespace atlas {

class AtlasCoapClient
{

public:
    
    /**
    * @brief Get CoAP client instance
    * @return CoAP client instance
    */
    static AtlasCoapClient &getInstance();

    /**
    * @brief Send CoAP client request
    * @param[in] uri CoAP URI
    * @param[in] method CoAP method
    * @param[in] reqPayload CoAP client request payload
    * @param[in] reqPayloadLen CoAP client request payload length
    * @param[in] timeout Timeout in ms
    * @param[in] callback CoAP request callback
    * @return Pointer to coap request token which can be used to cancel the request
    */
    
    void* sendRequest(const std::string &uri, AtlasCoapMethod method, const uint8_t *reqPayload,
                     size_t reqPayloadLen, uint32_t timeout,
                     coap_request_callback_t callback);

    /**
    * @brief Cancel CoAP client request
    * @param[in] coap request token
    * @return none
    */
    
    void cancelRequest(void *ctx);

    /**
    * @brief Set DTLS information
    * @param[in] identity DTLS identity
    * @param[in] DTLS Pre-shared key
    * @return none
    */
    void setDtlsInfo(const std::string &identity, const std::string &psk);

    AtlasCoapClient(const AtlasCoapClient &) = delete;
    AtlasCoapClient & operator=(const AtlasCoapClient &) = delete;

private:
    /**
    * @brief Default ctor for CoAP client
    * @return none
    */
    AtlasCoapClient() : token_(0) {}
    
    /**
    * @brief Resolve CoAP hostname
    * @param[in] hostname CoAP hostname
    * @param[out] dst Destination socket
    * @return Destination address length or -1 in case of error 
    */
    int resolveAddress(coap_str_const_t *hostname, struct sockaddr *dst);

    /**
    * @brief Create CoAP session
    * @param[in] ctx CoAP context
    * @param[in] proto CoAP protocol (UDP or DTLS)
    * @param[in] dst Destination address
    * @return CoAP session
    */
    coap_session_t *getSession(coap_context_t *ctx, coap_proto_t proto, coap_address_t *dst);

    static int eventHandler(coap_context_t *ctx, coap_event_t event, struct coap_session_t *session);

    static void nackHandler(coap_context_t *context, coap_session_t *session,
                     coap_pdu_t *sent, coap_nack_reason_t reason,
                     const coap_tid_t id);

    static void messageHandler(struct coap_context_t *ctx,
                               coap_session_t *session,
                               coap_pdu_t *sent,
                               coap_pdu_t *received,
                               const coap_tid_t id);

    void addRequest(coap_context_t *ctx, coap_session_t *session,
                    uint32_t token, uint32_t timeout,
                    coap_request_callback_t callback);


    void scheduleCallback(coap_context_t *ctx, int fd);
    
    void messageProcess(struct coap_context_t *ctx,
                        coap_session_t *session,
                        coap_pdu_t *sent,
                        coap_pdu_t *received,
                        const coap_tid_t id);
   
    bool validateToken(coap_pdu_t *received, uint32_t token);
    
    void alarmCallback(coap_context_t *ctx, int coapFd);
    
    /**
    * @brief Get CoAP request unique token
    * @return token
    */
    uint32_t getToken();

    /* DTLS identity */
    std::string dtlsIdentity_;

    /* DTLS key */
    std::string dtlsKey_;

    /* DTLS pre-shared key structure */
    coap_dtls_cpsk_t dtlsPsk_;

    /* CoAP client request callback */
    coap_request_callback_t callback_;

    /* CoAP request unique token */
    uint32_t token_;

    /* CoAP requests */
    std::unordered_map<coap_context_t*, AtlasCoapRequest> requests_;

    /* CoAP timeouts */
    std::unordered_map<coap_context_t*, std::unique_ptr<AtlasAlarm> > timeouts_;
};

} // namespace atlas

#endif /* __ATLAS_COAP_CLIENT_H__ */
