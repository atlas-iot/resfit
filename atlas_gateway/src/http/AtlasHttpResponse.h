#ifndef __ATLAS_HTTP_RESPONSE_H__
#define __ATLAS_HTTP_RESPONSE_H__

#include <boost/optional.hpp>

namespace atlas {

class AtlasHttpResponse {

public:
    /**
     * @brief Ctor for HTTP response
     * @param[in] statusCode HTTP status code
     * @return none
     */
    AtlasHttpResponse(int statusCode);
    
    /**
     * @brief Ctor for HTTP response
     * @param[in] statusCode HTTP status code
     * @param[in] payload HTTP response payload
     * @return none
     */
    AtlasHttpResponse(int statusCode, const std::string &payload);

    /**
     * @brief Get status code
     * @return HTTP status code
     */
    int getStatusCode() const { return statusCode_; }

    /**
     * @brief Get payload
     * @return HTTP response payload
     */
    boost::optional<std::string> getPayload() const { return payload_; }

private:
    /* HTTP response status code */
    int statusCode_;
    
    /* HTTP response payload */
    boost::optional<std::string> payload_;
};

} // namespace atlas

#endif /* __ATLAS_HTTP_RESPONSE_H__ */
