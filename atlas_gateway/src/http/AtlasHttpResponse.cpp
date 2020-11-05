#include "AtlasHttpResponse.h"

namespace atlas {

AtlasHttpResponse::AtlasHttpResponse(int statusCode) : statusCode_(statusCode) {}

AtlasHttpResponse::AtlasHttpResponse(int statusCode, const std::string &payload) : statusCode_(statusCode),
                                                                                   payload_(payload) {}

} // namespace atlas
