#include "AtlasHttpCallback.h"

namespace atlas {

AtlasHttpCallback::AtlasHttpCallback() : method_(AtlasHttpMethod::ATLAS_HTTP_GET), path_(""), handler_(nullptr) {}

AtlasHttpCallback::AtlasHttpCallback(AtlasHttpMethod method,
                                     const std::string &path,
                                     AtlasHttpHandler handler): method_(method), path_(path), handler_(handler) {}

} // namespace atlas
