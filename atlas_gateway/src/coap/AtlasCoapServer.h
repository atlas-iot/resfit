#ifndef __ATLAS_COAP_SERVER_H__
#define __ATLAS_COAP_SERVER_H__

#include <coap2/coap.h>
#include <string>
#include <unordered_map>
#include "AtlasCoapResource.h"

#define ATLAS_MAX_PSK_KEY_BYTES (64)

namespace atlas {

enum AtlasCoapServerMode {
    /* UDP transport mode */
    ATLAS_COAP_SERVER_MODE_UDP = (1 << 0),
 
    /* DTLS with PSK transport mode */
    ATLAS_COAP_SERVER_MODE_DTLS_PSK = (1 << 1),
 
    /* UDP + DTLS PSK transport modes */
    ATLAS_COAP_SERVER_MODE_BOTH = (ATLAS_COAP_SERVER_MODE_UDP | ATLAS_COAP_SERVER_MODE_DTLS_PSK),
};

class AtlasCoapServer {

public:

    /**
     * @brief Instance getter for CoAP server
     * @return CoAP server instance
     */
    static AtlasCoapServer& getInstance();

    /**
     * @brief Start CoAP server
     * @param[in] port CoAP port
     * @param[in] mode Server mode (UDP, DTLS or both)
     */
    void start(uint16_t port, AtlasCoapServerMode mode);


    /**
     * @brief Add CoAP resource
     * @param[in] resource CoAP resource
     * @return none
     */
    void addResource(const AtlasCoapResource &resource);
    
    /**
     * @brief Delete CoAP resource
     * @param[in] resource CoAP resource
     * @return none
     */
    void delResource(const AtlasCoapResource &resource);

    AtlasCoapServer(const AtlasCoapServer&) = delete;
    AtlasCoapServer &operator=(const AtlasCoapServer& ) = delete;

private:

    /**
     * @brief Default ctor
     * @return none
     */
    AtlasCoapServer();

    /**
     * @brief Create CoAP context
     * @param[in] hostname CoAP hostname
     * @param[in] port CoAP port
     * @param[in] mode Server mode
     * @return CoAP context
     */
    coap_context_t *getContext(uint16_t port, AtlasCoapServerMode mode);
    
    /**
     * @brief Set DTLS PSK info
     * @param[in] ctx CoAP context
     * @return none
     */
    void setDtlsPsk(coap_context_t *ctx);

    /**
     * @brief Init default CoAP resource
     * @param[in] ctx CoAP context
     * @return none
     */
    void initDefaultResources(coap_context_t *ctx);

    /**
     * @brief Getter for the default CoAP index handler
     * @param[in] ctx CoAP context
     * @param[in] resource CoAP resource
     * @param[in] session CoAP session
     * @param[in] request CoAP request
     * @param[in] token CoAP token
     * @param[in] query CoAP query
     * @param[out] response CoAP response
     * @return none
     */
    static void getDefaultIndexHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session,
                                coap_pdu_t *request, coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    /**
     * @brief Getter for the default CoAP GET handler
     * @param[in] ctx CoAP context
     * @param[in] resource CoAP resource
     * @param[in] session CoAP session
     * @param[in] request CoAP request
     * @param[in] token CoAP token
     * @param[in] query CoAP query
     * @param[out] response CoAP response
     * @return none
     */
    static void getHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    /**
     * @brief Getter for the default CoAP POST handler
     * @param[in] ctx CoAP context
     * @param[in] resource CoAP resource
     * @param[in] session CoAP session
     * @param[in] request CoAP request
     * @param[in] token CoAP token
     * @param[in] query CoAP query
     * @param[out] response CoAP response
     * @return none
     */
    static void postHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    /**
     * @brief Getter for the default CoAP PUT handler
     * @param[in] ctx CoAP context
     * @param[in] resource CoAP resource
     * @param[in] session CoAP session
     * @param[in] request CoAP request
     * @param[in] token CoAP token
     * @param[in] query CoAP query
     * @param[out] response CoAP response
     * @return none
     */
    static void putHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    /**
     * @brief Getter for the default CoAP DELETE handler
     * @param[in] ctx CoAP context
     * @param[in] resource CoAP resource
     * @param[in] session CoAP session
     * @param[in] request CoAP request
     * @param[in] token CoAP token
     * @param[in] query CoAP query
     * @param[out] response CoAP response
     * @return none
     */
    static void deleteHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                           coap_binary_t *token, coap_string_t *query, coap_pdu_t *response);

    /**
     * @brief Verify CoAP identity for DTLS session
     * @param[in] identity CoAP identity
     * @param[in] session CoAP session
     * @param[in] arg Unused argument
     * @return CoAP PSK for identity
     */
    static const coap_bin_const_t *verifyIdentity(coap_bin_const_t *identity, coap_session_t *session, void *arg);

    /**
     * @brief Verify CoAP identity for DTLS session
     * @param[in] identity CoAP identity
     * @return CoAP PSK for identity
     */
    const coap_bin_const_t *getPskForIdentity(coap_bin_const_t *identityVal);

    /**
     * @brief Handle CoAP request
     * @param[in] ctx CoAP context
     * @param[in] resource CoAP resource
     * @param[in] session CoAP session
     * @param[in] request CoAP request
     * @param[in] token CoAP token
     * @param[in] query CoAP query
     * @param[out] response CoAP response
     * @param[in] method CoAP method
     * @return none
     */
    void incomingHandler(coap_context_t *ctx, struct coap_resource_t *resource, coap_session_t *session, coap_pdu_t *request,
                         coap_binary_t *token, coap_string_t *query, coap_pdu_t *response, AtlasCoapMethod method);

    /**
     * @brief Schedule CoAP library when a file descriptor is available
     * @return none
     */
    void schedulerCallback(); 

    /* CoAP context */
    coap_context_t *ctx_;

    /* Hash map for keeping the server resources */
    std::unordered_map<std::string, AtlasCoapResource> resources_;

    /* PSK for the DTLS session */
    coap_bin_const_t identityPsk_;
};

} //namespace atlas

#endif /* __ATLAS_COAP_SERVER_H__ */
