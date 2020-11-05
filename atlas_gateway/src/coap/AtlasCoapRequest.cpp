#include "AtlasCoapRequest.h"

namespace atlas {

AtlasCoapRequest::AtlasCoapRequest(coap_context_t *context, coap_session_t *session,
                                   uint32_t token, coap_request_callback_t callback)
{
    context_ = context;
    session_ = session;
    callback_ = callback;
    token_ = token;
}

AtlasCoapRequest::AtlasCoapRequest() : context_(nullptr), session_(nullptr), callback_(nullptr) {}

} // namespace atlas
