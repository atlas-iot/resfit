#include "AtlasCoapResource.h"

namespace atlas {

AtlasCoapResource::AtlasCoapResource(const std::string &uri, AtlasCoapMethod method, resource_callback_t callback)
{
    this->uri_ = uri;
    this->method_ = method;
    this->callback_ = callback;
}

AtlasCoapResource::AtlasCoapResource() : uri_(""), method_(ATLAS_COAP_METHOD_GET), callback_(nullptr) {}

} // namespace atlas
