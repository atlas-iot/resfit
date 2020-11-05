#ifndef __ATLAS_HTTP_CALLBACK_H__
#define __ATLAS_HTTP_CALLBACK_H__

#include <string>
#include <functional>
#include "AtlasHttpMethod.h"
#include "AtlasHttpResponse.h"

namespace atlas {

typedef std::function<AtlasHttpResponse (AtlasHttpMethod, const std::string &, const std::string &)> AtlasHttpHandler;

class AtlasHttpCallback {

public:

    /**
     * @brief Default ctor for HTTP callback
     * @return none
     */
    AtlasHttpCallback();

    /**
     * @brief Ctor for HTTP callback
     * @param[in] method HTTP method
     * @param[in] path HTTP path
     * @param[in] handler HTTP handler
     * @return none
     */
    AtlasHttpCallback(AtlasHttpMethod method, const std::string &path, AtlasHttpHandler handler);

    /**
     * @brief Get HTTP method
     * @return HTTP method
     */
    AtlasHttpMethod getMethod() const { return method_; }

    /**
     * @brief Get HTTP path
     * @return HTTP path
     */
    std::string getPath() const { return path_; }

    /**
     * @brief Get HTTP handler
     * @return HTTP handler
     */
    AtlasHttpHandler getHandler() const { return handler_; }

private:
    /* HTTP method */
    AtlasHttpMethod method_;
    /* HTTP path */
    std::string path_;
    /* Callback for handling the request */ 
    AtlasHttpHandler handler_;
};

} // namespace atlas

#endif /* __ATLAS_HTTP_CALLBACK_H__ */
