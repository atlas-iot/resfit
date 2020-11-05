#ifndef __ATLAS_HTTP_SERVER_H__
#define __ATLAS_HTTP_SERVER_H__

#include <string>
#include <unordered_map>
#include <nghttp2/asio_http2_server.h>
#include "AtlasHttpCallback.h"

using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;

namespace atlas {

class AtlasHttpServer {

public:
    /**
     * @brief Get HTTP server instance
     * @return HTTP server instance
     */
    static AtlasHttpServer& getInstance();

    /**
     * @brief Start HTTP2 server
     * @return true if server is started, false otherwise
     */
    bool start(const std::string &certFile, const std::string &privKeyFile, int port);

    /**
     * @brief Stop HTTP server
     * @return none
     */
    void stop();

    /**
     * @brief Add HTTP callback (add callbacks before starting the HTTP server)
     * @param[in] httpCallback HTTP callback
     * @return true if callback is added, false otherwise
     */
    bool addCallback(const AtlasHttpCallback &httpCallback);

    /**
     * @brief Get HTTP server boost::asio service (the main assumption here is that the server runs on a single thread)
     * @return HTTP server service
     */
    inline boost::asio::io_service& getService() { assert(server_.io_services().size() == 1); return *server_.io_services()[0]; }

    /**
     * @brief Default dtor
     * @return none
     */
    ~AtlasHttpServer();

private:
    /**
     * @brief Default ctor
     * @return none
     */
    AtlasHttpServer();
    
    /**
     * @brief Copy ctor
     * @return none
     */
    AtlasHttpServer(const AtlasHttpServer&) = delete;

    /**
     * @brief Handle HTTP request
     * @param[in] req HTTP request
     * @param[in] res HTTP response 
     * @return none
     */
    void handleRequest(const request &req, const response &res);

    /* HTTP2 server */
    http2 server_;

    /* TLS context */
    boost::asio::ssl::context tls_;

    /* HTTP callbacks */
    std::unordered_map<std::string, AtlasHttpCallback> callbacks_;
};

} // namespace atlas

#endif /* __ATLAS_HTTP_SERVER_H__ */
